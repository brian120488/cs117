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


using namespace std;
using namespace C150NETWORK;

string make_hash(string file_name);
vector<string> split(const string &str, char delimiter, int limit);
C150DgmSocket* createSocket(int nastiness);
void checkAndPrintMessage(ssize_t readlen, char *msg, ssize_t bufferlen);
void processIncomingMessages(C150DgmSocket *sock, const string &programName, string targetdir, int file_nastiness);
bool sendMessageWithRetries(C150DgmSocket *sock, const string &msg, int maxRetries);

const int maxRetries = 5;
const int packetSize = 256;

struct Message {
    string command;
    string file_name;
    int byte_offset;
    char data[256];
    string hash;
};

int main(int argc, char *argv[]) {
    //
    // Check command line and parse arguments
    //
    if (argc != 4)  {
        fprintf(stderr,"Correct syntxt is: %s <networknastiness> <filenastiness> <targetdir>\n", argv[0]);
        exit(1);
    }
    if (strspn(argv[1], "0123456789") != strlen(argv[1])) {
        fprintf(stderr,"Nastiness %s is not numeric\n", argv[1]);     
        fprintf(stderr,"Correct syntxt is: %s <networknastiness> <filenastiness> <targetdir>\n", argv[0]);     
        exit(4);
    }

    int network_nastiness = atoi(argv[1]);
    int file_nastiness = atoi(argv[2]);
    char* targetdir = argv[3];

    //
    //  DO THIS FIRST OR YOUR ASSIGNMENT WON'T BE GRADED!
    //
    GRADEME(argc, argv);

    //
    //  Set up debug message logging
    //
    // setUpDebugLogging("fileserverdebug.txt", argc, argv);.

    // c150debug->setIndent("    ");              // if we merge client and server
    try {
        // Create the socket
        C150DgmSocket *sock = createSocket(network_nastiness);
        processIncomingMessages(sock, argv[0], targetdir, file_nastiness);
        
    } catch(C150NetworkException& e) {
         // Write to debug log
        c150debug->printf(C150ALWAYSLOG,"Caught C150NetworkException: %s\n",
                          e.formattedExplanation().c_str());
        // In case we're logging to a file, write to the console too
        cerr << argv[0] << ": caught C150NetworkException: " << e.formattedExplanation() << endl;
    }

    return 0;
}


// Function to create and configure the socket
C150DgmSocket* createSocket(int nastiness) {
    c150debug->printf(C150APPLICATION,"Creating C150NastyDgmSocket(nastiness=%d)", nastiness);

    C150DgmSocket *sock = new C150NastyDgmSocket(nastiness);
    sock->turnOnTimeouts(500);  // Set a 500ms timeout

    c150debug->printf(C150APPLICATION,"Socket ready to accept messages");

    return sock;
}

// Function to process incoming messages and handle responses
void processIncomingMessages(C150DgmSocket *sock, const string &programName, string targetdir, int file_nastiness) {
    ssize_t readlen;
    char incomingMessage[512];
    string return_msg;

    while (1) {
        readlen = sock->read(incomingMessage, sizeof(incomingMessage) - 1);
        if (readlen == 0) {
            c150debug->printf(C150APPLICATION,"Read zero length message, trying again");
            continue;
        }
        incomingMessage[readlen] = '\0';  // Ensure null termination
        
        string incoming(incomingMessage);
        vector<string> arguments = split(incoming, ' ', 4);

        // Checks file and sends back checksum
        if (arguments[0] == "CHECK") {
            string file_name = arguments[1];
            string file_path = targetdir + "/" + file_name;
            string hash = make_hash(file_path);
            
            // Grade log receiving file 
            *GRADING << "File: " << file_name << " starting to receive file" << endl;
            return_msg = "CHECK " + file_name + " " + hash;
            *GRADING << "File: " << file_name << " received, beginning end-to-end check" << endl;

            sock->write(return_msg.c_str(), return_msg.length() + 1);  // +1 includes the null terminator
        } else if (arguments[0] == "EQUAL") {
            cout << arguments[1]  + " " + arguments[2] << endl;
            string file_name = arguments[1];
            bool is_equal = (arguments[2] == "1");

            *GRADING << "File: " << arguments[1] << " end-to-end check ";
            *GRADING << ((is_equal) ? "succeeded" : "failed");
            *GRADING << endl;

            //TODO: The server should acknowledge to the client
            return_msg = incomingMessage;
            sock->write(return_msg.c_str(), return_msg.length() + 1);
        } else if (arguments[0] == "COPY") {
            string file_name = arguments[1];
            int byte_offset = stoi(arguments[2]);
            // string hash = arguments[3];
            // cout << hash << endl;
            string data = arguments[4];

            // Open the file for writing (create or overwrite by default)
            string file_path = targetdir + "/" + file_name + ".TMP";
            NASTYFILE outputFile(file_nastiness); 

            void *fopenretval = outputFile.fopen(file_path.c_str(), "r+b");
            if (fopenretval == NULL) {
                // If file doesn't exist, open it in "w+b" mode to create it
                fopenretval = outputFile.fopen(file_path.c_str(), "w+b");
                if (fopenretval == NULL) {
                    cerr << "Error creating and opening file " << file_path << " errno=" << strerror(errno) << endl;
                    exit(12);
                }
            }
            outputFile.fseek(byte_offset, SEEK_SET);
            char arr[packetSize] = {0};
            int length = data.copy(arr, data.length());
            int len = outputFile.fwrite(arr, 1, length);
            cout << length << " " << len << endl;
            
            if (len != length) {
                cerr << "Error writing file " << file_path << 
                    "  errno=" << strerror(errno) << endl;
                exit(16);
            }

            // TODO: close at last packet
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

        // c150debug->printf(C150APPLICATION, "Returned from write, doing read()");

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

// std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
//     std::vector<std::string> tokens;
//     size_t pos = 0;
//     std::string token;
//     while ((pos = s.find(delimiter)) != std::string::npos) {
//         token = s.substr(0, pos);
//         tokens.push_back(token);
//         s.erase(0, pos + delimiter.length());
//     }
//     tokens.push_back(s);

//     return tokens;
// }

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
    // cout << "File path in hash func: " << file_path << endl;
    std::ifstream t(file_path, std::ios::binary);
    if (!t.is_open()) {
        throw std::runtime_error("Unable to open file: " + file_path);
    }

    // Read the entire file content into a string
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string content = buffer.str();

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


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                     setUpDebugLogging
//
//        For COMP 150-IDS, a set of standards utilities
//        are provided for logging timestamped debug messages.
//        You can use them to write your own messages, but 
//        more importantly, the communication libraries provided
//        to you will write into the same logs.
//
//        As shown below, you can use the enableLogging
//        method to choose which classes of messages will show up:
//        You may want to turn on a lot for some debugging, then
//        turn off some when it gets too noisy and your core code is
//        working. You can also make up and use your own flags
//        to create different classes of debug output within your
//        application code
//
//        NEEDSWORK: should be factored into shared code w/pingserver
//        NEEDSWORK: document arguments
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 
void setUpDebugLogging(const char *logname, int argc, char *argv[]) {

    //   
    //           Choose where debug output should go
    //
    // The default is that debug output goes to cerr.
    //
    // Uncomment the following three lines to direct
    // debug output to a file. Comment them
    // to default to the console.
    //
    // Note: the new DebugStream and ofstream MUST live after we return
    // from setUpDebugLogging, so we have to allocate
    // them dynamically.
    //
    //
    // Explanation: 
    // 
    //     The first line is ordinary C++ to open a file
    //     as an output stream.
    //
    //     The second line wraps that will all the services
    //     of a comp 150-IDS debug stream, and names that filestreamp.
    //
    //     The third line replaces the global variable c150debug
    //     and sets it to point to the new debugstream. Since c150debug
    //     is what all the c150 debug routines use to find the debug stream,
    //     you've now effectively overridden the default.
    //
    ofstream *outstreamp = new ofstream(logname);
    DebugStream *filestreamp = new DebugStream(outstreamp);
    DebugStream::setDefaultLogger(filestreamp);

    //
    //  Put the program name and a timestamp on each line of the debug log.
    //
    c150debug->setPrefix(argv[0]);
    c150debug->enableTimestamp(); 

    //
    // Ask to receive all classes of debug message
    //
    // See c150debug.h for other classes you can enable. To get more than
    // one class, you can or (|) the flags together and pass the combined
    // mask to c150debug -> enableLogging 
    //
    // By the way, the default is to disable all output except for
    // messages written with the C150ALWAYSLOG flag. Those are typically
    // used only for things like fatal errors. So, the default is
    // for the system to run quietly without producing debug output.
    //
    c150debug->enableLogging(C150APPLICATION | C150NETWORKTRAFFIC | 
                             C150NETWORKDELIVERY); 
}