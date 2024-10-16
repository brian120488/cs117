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
#include <unistd.h>
#include <csignal>

using namespace std;
namespace fs = filesystem;
using namespace C150NETWORK;

struct Message {
    string command;
    string file_name;
    int byte_offset;
    char data[440];
    string hash;
};

const int PACKET_SIZE = 440;
const int MAX_RETRIES = 5;

void checkArguments(int argc, char *argv[]);
string make_hash(string file_name);
ssize_t write_to_server_and_wait(C150DgmSocket *sock, string message, char *incomingMessage);
vector<string> split(const string &str, char delimiter);
void checkAndPrintMessage(ssize_t readlen, char *msg, ssize_t bufferlen);
void openFile(NASTYFILE &file, string file_path, string mode);
void closeFile(NASTYFILE &file, string file_path);
void copyFile(C150DgmSocket *sock, NASTYFILE &file, string file_name);
bool checkFile(C150DgmSocket *sock, string file_name, string file_path);
string make_data_hash(string data);


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

    // read file - filecopy
    for (const auto& entry : fs::directory_iterator(srcdir)) {
        NASTYFILE file(file_nastiness);  
        string file_name = entry.path().filename().string();
        string file_path = string(srcdir) + "/" + file_name;
        openFile(file, file_path, "rb");

        bool isValid = false;
        for (int retries = 0; retries < 5 and !isValid; retries++) {
            *GRADING << "File: " << file_name << ", beginning transmission, attempt " << retries + 1 << endl;
            copyFile(sock, file, file_name);
            *GRADING << "File: " << file_name << " transmission complete, waiting for end-to-end check, attempt " << retries + 1 << endl;
            isValid = checkFile(sock, file_name, file_path);
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
    int maxRetries = 5;
    bool messageReceived = false;
    string file_name = split(message, ' ')[1];
    for (int retries = 0; retries < maxRetries && !messageReceived; retries++) {
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

void openFile(NASTYFILE &file, string file_path, string mode) {
    void *fopenretval = file.fopen(file_path.c_str(), "rb");  
    if (fopenretval == NULL) {
        cerr << "Error opening input file " << file_path << 
            " errno=" << strerror(errno) << endl;
        exit(12);
    }
}

void closeFile(NASTYFILE &file, string file_path) {
    if (file.fclose() != 0) {
        cerr << "Error closing input file " << file_path << 
            " errno=" << strerror(errno) << endl;
        exit(16);
    }
}

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

void copyFile(C150DgmSocket *sock, NASTYFILE &file, string file_name) {
    char buffer[PACKET_SIZE];
    int byte_offset = 0;
    int len;
    while ((len = file.fread(buffer, 1, PACKET_SIZE))) {
        if (len > PACKET_SIZE) {
            cerr << "Error reading file " << file_name << 
                "  errno=" << strerror(errno) << endl;
            exit(16);
        }

        string bufferString(buffer);
        bufferString = bufferString.substr(0, len);
        string hash = make_data_hash(bufferString);
        string message = "COPY " + file_name + " ";
        message += to_string(byte_offset * PACKET_SIZE) + " " + hash + " ";
        message += bufferString.substr(0, len);

        char incomingMessage[512];
        bool isValidHash = false;
        for (int retries = 0; retries < MAX_RETRIES and !isValidHash; retries++) {
            *GRADING << "File: " << file_name << " sending bytes " 
                << byte_offset * PACKET_SIZE << "-" << (byte_offset + 1) * PACKET_SIZE - 1 << endl;
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

bool checkFile(C150DgmSocket *sock, string file_name, string file_path) {
    string hash = make_hash(file_path);

    char incomingMessage[512];
    ssize_t readlen;

    vector<string> arguments;
    bool isCheckPacket = false;
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
    string server_file_name = arguments[1];
    string incoming_hash = arguments[2];

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
        exit(0);
    } 

    return (incoming_hash == hash);
}

