// 
//            fileclient.cpp
//
//     Author: Brian Yang and Manuel Pena
//
//     Program for client to send a file over a socket
//     to a different computer running the fileserver 
//     program.
//
//     
//

#include <string>
#include <stdlib.h>
#include "c150grading.h"
#include <filesystem>
#include "c150nastyfile.h"
#include <fstream> 
#include <dirent.h>
#include <sys/stat.h>
#include "c150nastydgmsocket.h"
#include <unistd.h>
#include <csignal>
#include "processing.h"

using namespace std;
namespace fs = filesystem;
using namespace C150NETWORK;

const int PACKET_SIZE = 440;
const int MAX_RETRIES = 5;

void checkArguments(int argc, char *argv[]);
ssize_t write_to_server_and_wait(C150DgmSocket *sock, string message, char *incomingMessage);
void checkAndPrintMessage(ssize_t readlen, char *msg, ssize_t bufferlen);
void openFile(NASTYFILE &file, string file_path, string mode);
void closeFile(NASTYFILE &file, string file_path);
void copyFile(C150DgmSocket *sock, NASTYFILE &file, string file_name);
bool checkFile(C150DgmSocket *sock, string file_name, string file_path, int retries);


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                         main
//
//         Main file for the fileclient program
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int main(int argc, char *argv[]) {
    GRADEME(argc, argv);
    checkArguments(argc, argv);

    char* server = argv[1];
    int network_nastiness = atoi(argv[2]);
    int file_nastiness = atoi(argv[3]);
    char* srcdir = argv[4]; 

    C150DgmSocket *sock = new C150NastyDgmSocket(network_nastiness);
    sock -> setServerName(server);  
    sock -> turnOnTimeouts(100);

    // read files in the source directory and transmit to server in target
    for (const auto& entry : fs::directory_iterator(srcdir)) {
        NASTYFILE file(file_nastiness);  
        string file_name = entry.path().filename().string();
        string file_path = string(srcdir) + "/" + file_name;
        openFile(file, file_path, "rb");

        bool isValid = false;
        // Sends file and retires until MAX_RETRIES or END-TO-END check is valid
        for (int retries = 0; retries < MAX_RETRIES and !isValid; retries++) {
            *GRADING << "File: " << file_name << ", beginning transmission, attempt " << retries + 1 << endl;
            copyFile(sock, file, file_name);
            *GRADING << "File: " << file_name << " transmission complete, waiting for end-to-end check, attempt " << retries + 1 << endl;
            isValid = checkFile(sock, file_name, file_path, retries);
        }

        cout << file_name << ": copy " << (isValid ? "successful." : "failed.") << endl;
        closeFile(file, file_path);
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
    bool messageReceived = false;
    string file_name = split(message, ' ')[1];
    for (int retries = 0; retries < MAX_RETRIES && !messageReceived; retries++) {
        sock->write(message.c_str(), message.length() + 1); // +1 includes the null terminator
        
        readlen = sock->read(incomingMessage, 512);
        if (readlen == 0) continue;

        incomingMessage[readlen] = '\0';
        messageReceived = true;
    }
    return readlen;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                            openFile
//
//        Opens a file and checks if it is opened correctly
//        
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void openFile(NASTYFILE &file, string file_path, string mode) {
    void *fopenretval = file.fopen(file_path.c_str(), "rb");  
    if (fopenretval == NULL) {
        cerr << "Error opening input file " << file_path << 
            " errno=" << strerror(errno) << endl;
        exit(12);
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                            closeFile
//
//        Closes a file and checks it closed correctly
//        
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void closeFile(NASTYFILE &file, string file_path) {
    if (file.fclose() != 0) {
        cerr << "Error closing input file " << file_path << 
            " errno=" << strerror(errno) << endl;
        exit(16);
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                           checkArguments
//
//        Checks if the commands are correct when running.
//        
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void checkArguments(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr,"Correct syntax is %s <server> <networknastiness> <filenastiness> <srcdir>\n", argv[0]);
        exit (4);
    }

    if (strspn(argv[2], "0123456789") != strlen(argv[2]) || strspn(argv[3], "0123456789") != strlen(argv[3])) {
        fprintf(stderr,"Nastiness %s or %s is not numeric\n", argv[2], argv[3]);    
        exit(4);
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                            copyFile
//
//        Handles the COPY header packet to send to server
//        COPY packet includes:
//          Header:      COPY
//          File name:   NASTYFILE &file
//          File offset: byte_offset 
//          Hash:        hash  
//          Data:        bufferString.susbtr(0, len)          
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void copyFile(C150DgmSocket *sock, NASTYFILE &file, string file_name) {
    char buffer[PACKET_SIZE];
    int byte_offset = 0;
    int len;

    // Reads until eof, when len of fread is 0
    while ((len = file.fread(buffer, 1, PACKET_SIZE))) {
        if (len > PACKET_SIZE) {
            cerr << "Error reading file " << file_name << 
                "  errno=" << strerror(errno) << endl;
            exit(16);
        }
        
        // Building COPY packet
        string bufferString(buffer);
        bufferString = bufferString.substr(0, len);
        string hash = make_data_hash(bufferString);
        string message = "COPY " + file_name + " ";
        message += to_string(byte_offset * PACKET_SIZE) + " " + hash + " ";
        message += bufferString.substr(0, len);

        char incomingMessage[512];
        bool isValidHash = false;

        // If the server returns invalid hash we will retry MAX_RETRIES 
        for (int retries = 0; retries < MAX_RETRIES and !isValidHash; retries++) {
            ssize_t readlen = write_to_server_and_wait(sock, message, incomingMessage);
            if (readlen == 0) {
                printf("Server not responding\n");
                exit(0);
            } 
            incomingMessage[readlen] = '\0';

            string server_hash(incomingMessage);
            isValidHash = (hash == server_hash);
        }

        if (!isValidHash) {
            printf("Server not responding with correct hash\n");
            return;
        }

        byte_offset++;
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                            checkFile
//
//        Sends a packet that tells the server to perform
//        END-TO-END check on the file .TMP in the server
//        CHECK packet includes:
//          Header:    CHECK
//          File name: file_name
// 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool checkFile(C150DgmSocket *sock, string file_name, string file_path, int retries) {
    string hash = make_hash(file_path);
    char incomingMessage[512];
    ssize_t readlen;
    vector<string> arguments;
    bool isCheckPacket = false;

    // Send check packet until the server responds
    while (!isCheckPacket) {
        readlen = write_to_server_and_wait(sock, "CHECK " + file_name, incomingMessage);
        if (readlen == 0) {
            printf("Server not responding\n");
            exit(0);
        } 
        string incoming(incomingMessage);
        arguments = split(incoming, ' ');
        isCheckPacket = (arguments[0] == "CHECK");
    }

    // Get filename and hash from server from split above
    string server_file_name = arguments[1];
    string incoming_hash = arguments[2];

    // Send EQUAL packet to confirm if the CHECK is valid
    string msg = "EQUAL " + file_name + " ";
    msg += (incoming_hash == hash) ? "1" : "0";
    if (incoming_hash == hash) {                                            
        *GRADING << "File: " << file_name << " end-to-end check succeeded, attempt "
                                                            << retries << endl;
    } else {
        *GRADING << "File: " << file_name << " end-to-end check failed, attempt " 
                                                            << retries << endl;
    }

    readlen = write_to_server_and_wait(sock, msg, incomingMessage);
    if (readlen == 0) {
        printf("Server not responding\n");
        exit(0);
    } 

    // Return if the END-TO-END check failed 
    return (incoming_hash == hash);
}

