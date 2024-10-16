// 
//            fileserver.cpp
//
//     Author: Brian Yang and Manuel Pena
//
//     Program for server to receive a file over a socket
//     from a different computer running the fileclient 
//     program.
//
//     
//
#include "c150nastydgmsocket.h"
#include "c150debug.h"
#include "c150grading.h"
#include <fstream>
#include <cstdlib> 
#include <sstream> 
#include <iomanip> 
#include <vector>
#include "c150nastyfile.h"
#include <cstdio>
#include <filesystem>
#include <csignal>
#include "processing.h"

using namespace std;
namespace fs = filesystem;
using namespace C150NETWORK;

void checkArguments(int argc, char *argv[]);
vector<string> processMessage(char incomingMessage[512]);
C150DgmSocket* createSocket(int nastiness);
void processIncomingMessages(C150DgmSocket *sock, const string &programName, string targetdir, int file_nastiness);
void open_or_create_file(NASTYFILE &file, string file_path);
void checkFile(vector<string> arguments, string targetdir, C150DgmSocket *sock, ofstream *GRADING);
void checkEqual(vector<string> arguments, string targetdir, C150DgmSocket *sock, ofstream *GRADING);
void copyFile(vector<string> arguments, string targetdir, C150DgmSocket *sock, ofstream *GRADING, int file_nastiness);

const int maxRetries = 5;
const int PACKET_SIZE = 440;

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
        cerr << argv[0] << ": caught C150NetworkException: " << e.formattedExplanation() << endl;
    }

    return 0;
}

// Function to process incoming messages and handle responses
void processIncomingMessages(C150DgmSocket *sock, const string &programName, string targetdir, int file_nastiness) {
    char incomingMessage[512];

    while (1) {
        ssize_t readlen = sock->read(incomingMessage, sizeof(incomingMessage) - 1);
        if (readlen == 0) continue;
        incomingMessage[readlen] = '\0';  // Ensure null termination

        vector<string> arguments = processMessage(incomingMessage);
        if (arguments[0] == "CHECK") { // Checks file and sends back checksum
            checkFile(arguments, targetdir, sock, GRADING);
        } else if (arguments[0] == "EQUAL") {
            checkEqual(arguments, targetdir, sock, GRADING);
        } else if (arguments[0] == "COPY") {
            copyFile(arguments, targetdir, sock, GRADING, file_nastiness);
        } else {
            cout << "Invalid message" << endl;
        }
    }
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                         checkArguments
//
//        Checks that the program is given 4 commands
//        
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
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

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                            processMessage
//
//        Process data received from the client
//        
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
vector<string> processMessage(char incomingMessage[512]) {    
    string incoming(incomingMessage);
    vector<string> arguments = split_with_limit(incoming, ' ', 4);
    return arguments;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                        open_or_create_file
//
//        Opens a file if it exists or creates a new one
//        
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
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

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                        checkFile
//
//        Receives a packet that from the client to perform
//        END-TO-END check on the file .TMP in the server.
//        CHECK packet includes:
//          Header:    CHECK
//          File name: file_name
//        Returns via the socket a string of the hash of .TMP file 
//        to the client
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void checkFile(vector<string> arguments, string targetdir, C150DgmSocket *sock, ofstream *GRADING) {
    string file_name = arguments[1];
    string file_path = targetdir + "/" + file_name + ".TMP";
    if (!fs::exists(file_path)) return;
    string hash = make_hash(file_path);
    
    // Grade log receiving file 
    string return_msg = "CHECK " + file_name + " " + hash;
    *GRADING << "File: " << file_name << " received, beginning end-to-end check" << endl;

    sock->write(return_msg.c_str(), return_msg.length() + 1);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                        checkEqual
//
//        Receives a EQUAL packet from the client on END-TO-END
//        results. Renames .TMP file and confirms message
//        to client.
//        EQUAL packet includes:
//          Header:    EQUAL
//          File name: file_name
//          Is equal:  is_equal
//        Returns same message from client via the socket.
//        
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void checkEqual(vector<string> arguments, string targetdir, C150DgmSocket *sock, ofstream *GRADING) {
    string file_name = arguments[1];
    string file_path = targetdir + "/" + file_name;
    
    bool is_equal = (arguments[2] == "1");
    if (is_equal && fs::exists(file_path + ".TMP")) {
        string old_file_name = file_path + ".TMP";
        rename(old_file_name.c_str(), file_path.c_str());
    }

    *GRADING << "File: " << arguments[1] << " end-to-end check ";
    *GRADING << (is_equal ? "succeeded" : "failed") << endl;

    string return_msg = "EQUAL " + file_name + " " + arguments[2];
    sock->write(return_msg.c_str(), return_msg.length() + 1);
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                            copyFile
//
//        Handles the COPY header packet received from the client
//        COPY packet includes:
//          Header:      COPY
//          File name:   NASTYFILE &file
//          File offset: byte_offset 
//          Hash:        hash  
//          Data:        File data from client
//        Returns a hash of the Data from the packet to the
//        client.         
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void copyFile(vector<string> arguments, string targetdir, C150DgmSocket *sock, ofstream *GRADING, int file_nastiness) {
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
    char arr[PACKET_SIZE] = {0};
    int length = data.copy(arr, data.length());
    int len = outputFile.fwrite(arr, 1, length);

    if (byte_offset == 0) {
        *GRADING << "File: " << file_name << " starting to receive file" << endl;
    }
    
    if (len != length) {
        cerr << "Error writing file " << file_path << 
            "  errno=" << strerror(errno) << endl;
        exit(16);
    }

    outputFile.fclose();
}