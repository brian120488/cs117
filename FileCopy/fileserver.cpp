#include "c150nastydgmsocket.h"
#include "c150debug.h"
#include "c150grading.h"
#include <fstream>
#include <cstdlib> 
#include <sstream> 
#include <iomanip> 
#include <openssl/sha.h>  // For SHA1
#include <vector>
#include "c150nastyfile.h"
#include <cstdio>
#include <filesystem>

using namespace std;
using namespace C150NETWORK;

void checkArguments(int argc, char *argv[]);
vector<string> processMessage(char incomingMessage[512]);
string make_hash(string file_name);
vector<string> split(const string &str, char delimiter, int limit);
C150DgmSocket* createSocket(int nastiness);
void checkAndPrintMessage(ssize_t readlen, char *msg, ssize_t bufferlen);
void processIncomingMessages(C150DgmSocket *sock, const string &programName, string targetdir, int file_nastiness);
bool sendMessageWithRetries(C150DgmSocket *sock, const string &msg, int maxRetries);
void open_or_create_file(NASTYFILE &file, string file_path);
string make_data_hash(string data);

const int maxRetries = 5;
const int packetSize = 440;

struct Message {
    string command;
    string file_name;
    int byte_offset;
    char data[440];
    string hash;
};

int main(int argc, char *argv[]) {
    GRADEME(argc, argv);
    checkArguments(argc, argv);

    int network_nastiness = atoi(argv[1]);
    int file_nastiness = atoi(argv[2]);
    char* targetdir = argv[3];


    try {
        C150DgmSocket *sock = new C150NastyDgmSocket(network_nastiness);
        sock->turnOnTimeouts(100);  
        processIncomingMessages(sock, argv[0], targetdir, file_nastiness);
    } catch(C150NetworkException& e) {
        c150debug->printf(C150ALWAYSLOG,"Caught C150NetworkException: %s\n",
                          e.formattedExplanation().c_str());
        cerr << argv[0] << ": caught C150NetworkException: " << e.formattedExplanation() << endl;
    }

    return 0;
}

// Function to process incoming messages and handle responses
void processIncomingMessages(C150DgmSocket *sock, const string &programName, string targetdir, int file_nastiness) {
    char incomingMessage[512];
    string return_msg;

    while (1) {
        ssize_t readlen = sock->read(incomingMessage, sizeof(incomingMessage) - 1);
        if (readlen == 0) continue;

        incomingMessage[readlen] = '\0';  // Ensure null termination
        vector<string> arguments = processMessage(incomingMessage);

        cout << arguments[0] << " " << arguments[1] << endl;

        // Checks file and sends back checksum
        if (arguments[0] == "CHECK") {
            string file_name = arguments[1];
            string file_path = targetdir + "/" + file_name + ".TMP";
            string hash = make_hash(file_path);
            
            // Grade log receiving file 
            *GRADING << "File: " << file_name << " starting to receive file" << endl;
            return_msg = "CHECK " + file_name + " " + hash;
            *GRADING << "File: " << file_name << " received, beginning end-to-end check" << endl;

            sock->write(return_msg.c_str(), return_msg.length() + 1);  // +1 includes the null terminator

        } else if (arguments[0] == "EQUAL") {
            cout << arguments[1]  + " " + arguments[2] << endl;
            string file_name = arguments[1];
            string file_path = targetdir + "/" + file_name;
            
            bool is_equal = (arguments[2] == "1");
            if (is_equal && filesystem::exists(file_path + ".TMP")) {
                string old_file_name = file_path + ".TMP";
                rename(old_file_name.c_str(), file_path.c_str());
            }

            *GRADING << "File: " << arguments[1] << " end-to-end check ";
            *GRADING << ((is_equal) ? "succeeded" : "failed");
            *GRADING << endl;

            return_msg = incomingMessage;
            sock->write(return_msg.c_str(), return_msg.length() + 1);
        } else if (arguments[0] == "COPY") {
            string file_name = arguments[1];
            int byte_offset = stoi(arguments[2]);
            string client_hash = arguments[3];
            string data = arguments[4];
            
            string server_hash = make_data_hash(data);
            
            string return_msg = server_hash;
            sock->write(return_msg.c_str(), return_msg.length() + 1);

            // Open the file for writing (create or overwrite by default)
            NASTYFILE outputFile(file_nastiness); 
            string file_path = targetdir + "/" + file_name + ".TMP";
            open_or_create_file(outputFile, file_path);
            
            outputFile.fseek(byte_offset, SEEK_SET);
            char arr[packetSize] = {0};
            int length = data.copy(arr, data.length());
            int len = outputFile.fwrite(arr, 1, length);
            
            if (len != length) {
                cerr << "Error writing file " << file_path << 
                    "  errno=" << strerror(errno) << endl;
                exit(16);
            }


            // TODO: close at last packet or have CHECK close
            // if (outputFile.fclose() == 0) {
            //     cout << "Finished writing file " << file_name <<endl;
            // } else {
            //     cerr << "Error closing output file " << file_name << 
            //     " errno=" << strerror(errno) << endl;
            //     exit(16);
            // }

        } else {
            cout << "Invalid message" << endl;
        }

        // bool success = sendMessageWithRetries(sock, hash, maxRetries);
        // if (!success) {
        //     throw C150NetworkException("Server is not responding after multiple attempts.");
        // }
    }
}

// Function to send a message and handle retries
bool sendMessageWithRetries(C150DgmSocket *sock, const string &msg, int maxRetries) {
    ssize_t readlen;
    char incomingMessage[512];
    int retries = 0;
    bool messageReceived = false;

    while (retries < maxRetries && !messageReceived) {
        c150debug->printf(C150APPLICATION, "Writing message: \"%s\"", msg.c_str());
        sock->write(msg.c_str(), msg.length() + 1); // +1 includes the null terminator

        c150debug->printf(C150APPLICATION, "Returned from write, doing read()");
        readlen = sock->read(incomingMessage, sizeof(incomingMessage));

        if (sock->timedout()) {
            retries++;
            c150debug->printf(C150APPLICATION, "Timeout occurred. Retrying (%i/%i)...\n", retries, maxRetries);
        } else {
            messageReceived = true;
            checkAndPrintMessage(readlen, incomingMessage, sizeof(incomingMessage));

            // Keep reading additional responses until timeout
            while (1) {
                readlen = sock->read(incomingMessage, sizeof(incomingMessage));
                if (sock->timedout()) {
                    c150debug->printf(C150APPLICATION, "No more packets received, timing out.\n");
                    break;
                }
                checkAndPrintMessage(readlen, incomingMessage, sizeof(incomingMessage));
            }
        }
    }

    return messageReceived;
}

vector<string> split(const string &str, char delimiter, int limit = -1) {
     vector<string> tokens;
    istringstream stream(str);
    string token;
    int count = 0;
    size_t pos = 0, prev_pos = 0;
    
    // Split tokens based on the delimiter
    while (count != limit && (pos = str.find(delimiter, prev_pos)) != string::npos) {
        tokens.push_back(str.substr(prev_pos, pos - prev_pos));
        prev_pos = pos + 1;  // Move past the delimiter
        count++;
    }
    
    // If we've reached the limit or there are no more delimiters, append the remaining part of the string
    if (prev_pos < str.size()) {
        tokens.push_back(str.substr(prev_pos));
    }

    return tokens;
}

string make_hash(string file_path) {
    // Open the file
    std::ifstream t(file_path, std::ios::binary);
    if (!t.is_open()) {
        throw std::runtime_error("Unable to open file: " + file_path);
    }

    // Read the entire file content into a string
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string content = buffer.str();

    // Close the file
    t.close();

    // Compute the SHA-1 hash
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(content.c_str()), content.length(), hash);

    // Convert the binary hash to a hex string
    std::stringstream hex_stream;
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        hex_stream << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    // Return the hex string
    return hex_stream.str();
}

string make_data_hash(string data) {
    // Compute the SHA-1 hash
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(data.c_str()), data.length(), hash);

    // Convert the binary hash to a hex string
    std::stringstream hex_stream;
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        hex_stream << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    // Return the hex string
    return hex_stream.str();
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                     checkAndPrintMessage
//
//        Make sure length is OK, clean up response buffer
//        and print it to standard output.
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 


void
checkAndPrintMessage(ssize_t readlen, char *msg, ssize_t bufferlen) {
    // 
    // Except in case of timeouts, we're not expecting
    // a zero length read
    //
    if (readlen == 0) {
        throw C150NetworkException("Unexpected zero length read in client");
    }

    // DEFENSIVE PROGRAMMING: we aren't even trying to read this much
    // We're just being extra careful to check this
    if (readlen > (int)(bufferlen)) {
        throw C150NetworkException("Unexpected over length read in client");
    }

    //
    // Make sure server followed the rules and
    // sent a null-terminated string (well, we could
    // check that it's all legal characters, but 
    // at least we look for the null)
    //
    if(msg[readlen-1] != '\0') {
        throw C150NetworkException("Client received message that was not null terminated");     
    };

    //
    // Use a routine provided in c150utility.cpp to change any control
    // or non-printing characters to "." (this is just defensive programming:
    // if the server maliciously or inadvertently sent us junk characters, then we 
    // won't send them to our terminal -- some 
    // control characters can do nasty things!)
    //
    // Note: cleanString wants a C++ string, not a char*, so we make a temporary one
    // here. Not super-fast, but this is just a demo program.
    string s(msg);
    cleanString(s);

    // Echo the response on the console

    c150debug->printf(C150APPLICATION,"PRINTING RESPONSE: Response received is \"%s\"\n", s.c_str());
    printf("Response received is \"%s\"\n", s.c_str());

}

void checkArguments(int argc, char *argv[]) {
    if (argc != 4)  {
        fprintf(stderr,"Correct syntxt is: %s <networknastiness> <filenastiness> <targetdir>\n", argv[0]);
        exit(1);
    }
    if (strspn(argv[1], "0123456789") != strlen(argv[1])) {
        fprintf(stderr,"Nastiness %s is not numeric\n", argv[1]);     
        fprintf(stderr,"Correct syntxt is: %s <networknastiness> <filenastiness> <targetdir>\n", argv[0]);     
        exit(4);
    }
}

vector<string> processMessage(char incomingMessage[512]) {    
    string incoming(incomingMessage);
    vector<string> arguments = split(incoming, ' ', 4);
    return arguments;
}

void open_or_create_file(NASTYFILE &file, string file_path) {
    void *fopenretval = file.fopen(file_path.c_str(), "r+b");
    if (fopenretval == NULL) {
        // If file doesn't exist, open it in "w+b" mode to create it
        fopenretval = file.fopen(file_path.c_str(), "w+b");
        if (fopenretval == NULL) {
            cerr << "Error creating and opening file " << file_path << " errno=" << strerror(errno) << endl;
            exit(12);
        }
    }
}