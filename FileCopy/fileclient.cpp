#include <string>
#include <stdlib.h>
#include "c150grading.h"
#include <filesystem>
#include "c150nastyfile.h"
#include <fstream> 
#include <openssl/sha.h>
#include <dirent.h>
#include <sys/stat.h>
#include "c150nastydgmsocket.h"


using namespace std;
namespace fs = filesystem;
using namespace C150NETWORK;

struct Message {
    string command;
    string file_name;
    int byte_offset;
    char data[256];
    string hash;
    // string[] arguments;
};

string make_hash(string file_name);
void print_hash(const string& obuf);
ssize_t write_to_server_and_wait(C150DgmSocket *sock, string message, char *incomingMessage);
vector<string> split(const string &str, char delimiter);
void checkAndPrintMessage(ssize_t readlen, char *msg, ssize_t bufferlen);

int main(int argc, char *argv[]) {

    //
    //  DO THIS FIRST OR YOUR ASSIGNMENT WON'T BE GRADED!
    //
    GRADEME(argc, argv);

    if (argc != 5) {
        fprintf(stderr,"Correct syntax is %s <server> <networknastiness> <filenastiness> <srcdir>\n", argv[0]);
        exit (4);
    }

    if (strspn(argv[2], "0123456789") != strlen(argv[2]) || strspn(argv[3], "0123456789") != strlen(argv[3])) {
        fprintf(stderr,"Nastiness %s or %s is not numeric\n", argv[2], argv[3]);    
        exit(4);
    }

    char* server = argv[1];
    int network_nastiness = atoi(argv[2]);
    int file_nastiness = atoi(argv[3]);
    char* srcdir = argv[4]; 

    C150DgmSocket *sock = new C150NastyDgmSocket(network_nastiness);
    sock -> setServerName(server);  
    sock -> turnOnTimeouts(50);


    // read file - filecopy
    for (const auto& entry : fs::directory_iterator(srcdir)) {
        string file_name = entry.path().filename().string();
        if (file_name != "tyger.txt") {
            continue;
        }
        NASTYFILE file(file_nastiness);  
        
        string file_path = string(srcdir) + "/" + file_name;
        void *fopenretval = file.fopen(file_path.c_str(), "rb");  
        if (fopenretval == NULL) {
            cerr << "Error opening input file " << file_path << 
                " errno=" << strerror(errno) << endl;
            exit(12);
        }

        const int data_size = 256;
        char buffer[data_size];
        int byte_offset = 0;
        int len;
        while ((len = file.fread(buffer, 1, data_size))) {
            if (len > data_size) {
                cerr << "Error reading file " << file_name << 
                    "  errno=" << strerror(errno) << endl;
                exit(16);
            }

            // TODO: hash the packet data
            string hash = "1";
            string message = "COPY " + file_name + " ";
            message += to_string(byte_offset * data_size) + " " + hash + " ";
            message += string(buffer).substr(0, len);
            // message += '\0';
            cout << message;
            sock->write(message.c_str(), message.length() + 1);

            byte_offset++;
        }

        if (file.fclose() != 0) {
            cerr << "Error closing input file " << file_name << 
                " errno=" << strerror(errno) << endl;
            exit(16);
        }
    }







    return 0;

    try {
        // C150DgmSocket *sock = new C150NastyDgmSocket(network_nastiness);
        // sock -> setServerName(server);  
        // sock -> turnOnTimeouts(50);

        char incomingMessage[512];
        ssize_t readlen;
        for (const auto& entry : fs::directory_iterator(srcdir)) {
            string file_name = entry.path().filename().string();
            string hash = make_hash(entry.path().string());

            try {
                readlen = write_to_server_and_wait(sock, "CHECK " + file_name, incomingMessage);
                if (readlen == 0) {
                    printf("Server not responding\n");
                    break; 
                } 

                string incoming(incomingMessage);
                vector<string> arguments = split(incoming, ' ');
                string command = arguments[0];
                string server_file_name = arguments[1];
                string incoming_hash = arguments[2];
                
                // TODO: add while loop incase they send up wrong msg
                // if (command != "CHECK" or file_name != server_file_name) continue;

                string msg = "EQUAL " + file_name + " ";
                msg += (incoming_hash == hash) ? "1" : "0";
                if (incoming_hash == hash) {
                    *GRADING << "File: " << file_name << " end-to-end check succeeded, attempt 1" << endl;
                } else {
                    *GRADING << "File: " << file_name << " end-to-end check failed, attempt 1" << endl;
                }

                readlen = write_to_server_and_wait(sock, msg, incomingMessage);
                if (readlen == 0) {
                    printf("Server not responding\n");
                    break;
                } 

                cout << incomingMessage << endl;
            }
            catch (C150NetworkException& e) {
                c150debug->printf(C150ALWAYSLOG,"Caught C150NetworkException: %s\n",
                                e.formattedExplanation().c_str());
                cerr << argv[0] << ": caught C150NetworkException: " << e.formattedExplanation()
                                << endl;
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        cerr << "Filesystem error: " << e.what() << endl;
    } catch (const exception& e) {
        cerr << "General exception: " << e.what() << endl;
    }
    
    return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                         write_to_server_and_wait
//
//             Writes message to server and retries to get
//             message back from server
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
ssize_t write_to_server_and_wait(C150DgmSocket *sock, string message, char *incomingMessage) {
    ssize_t readlen;
    int maxRetries = 5;
    bool messageReceived = false;
    string file_name = split(message, ' ')[1];
    for (int retries = 0; retries < maxRetries && !messageReceived; retries++) {
        *GRADING << "File: " << file_name << ", beginning transmission, attempt " << retries + 1 << endl;
        sock->write(message.c_str(), message.length() + 1); // +1 includes the null terminator
        *GRADING << "File: " << file_name << " transmission complete, waiting for end-to-end check, attempt " << retries + 1 << endl;

        
        readlen = sock->read(incomingMessage, 512);
        if (readlen == 0) continue;

        incomingMessage[readlen] = '\0';
        // NEEDSWORK
        // while (!sock->timedout()) {
        //     readlen = sock -> read(incomingMessage, 512);
        // }
        messageReceived = true;
    }
    return readlen;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                            make_hash
//
//             Computes SHA-1 hash key for a given file 
//        
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
string make_hash(string file_path) {
    // Open the file
    ifstream t(file_path, ios::binary);
    if (!t.is_open()) {
        throw runtime_error("Unable to open file: " + file_path);
    }

    // Read the entire file content into a string
    stringstream buffer;
    buffer << t.rdbuf();
    string content = buffer.str();

    // Compute the SHA-1 hash
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(content.c_str()), content.length(), hash);

    // Convert the binary hash to a hex string
    stringstream hex_stream;
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        hex_stream << hex << setw(2) << setfill('0') << (int)hash[i];
    }

    // Return the hex string
    return hex_stream.str();
}

void print_hash(const string& obuf) {
    for (size_t i = 0; i < obuf.size(); i++) {
        printf("%02x", (unsigned char)obuf[i]);
    }
    cout << endl;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                     checkAndPrintMessage
//
//        Make sure length is OK, clean up response buffer
//        and print it to standard output.
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 


void checkAndPrintMessage(ssize_t readlen, char *msg, ssize_t bufferlen) {
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

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                            split
//
//        Split a string into tokens using a delimiter
//        
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
vector<string> split(const string &str, char delimiter) {
    vector<string> tokens;
    istringstream stream(str);
    string token;
    
    while (getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}