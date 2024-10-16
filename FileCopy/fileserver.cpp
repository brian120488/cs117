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
#include <csignal>

using namespace std;
namespace fs = filesystem;
using namespace C150NETWORK;

void checkArguments(int argc, char *argv[]);
vector<string> processMessage(char incomingMessage[512]);
string make_hash(string file_name);
vector<string> split(const string &str, char delimiter, int limit);
C150DgmSocket* createSocket(int nastiness);
void processIncomingMessages(C150DgmSocket *sock, const string &programName, string targetdir, int file_nastiness);
void open_or_create_file(NASTYFILE &file, string file_path);
string make_data_hash(string data);

const int maxRetries = 5;
const int PACKET_SIZE = 440;

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
        if (arguments[0] == "CHECK") { // Checks file and sends back checksum
            string file_name = arguments[1];
            string file_path = targetdir + "/" + file_name + ".TMP";
            if (!fs::exists(file_path)) continue;
            string hash = make_hash(file_path);
            
            // Grade log receiving file 
            *GRADING << "File: " << file_name << " starting to receive file" << endl;
            return_msg = "CHECK " + file_name + " " + hash;
            *GRADING << "File: " << file_name << " received, beginning end-to-end check" << endl;

            sock->write(return_msg.c_str(), return_msg.length() + 1);
        } else if (arguments[0] == "EQUAL") {
            string file_name = arguments[1];
            string file_path = targetdir + "/" + file_name;
            
            bool is_equal = (arguments[2] == "1");
            if (is_equal && fs::exists(file_path + ".TMP")) {
                string old_file_name = file_path + ".TMP";
                rename(old_file_name.c_str(), file_path.c_str());
            }

            *GRADING << "File: " << arguments[1] << " end-to-end check ";
            *GRADING << (is_equal ? "succeeded" : "failed") << endl;

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
            char arr[PACKET_SIZE] = {0};
            int length = data.copy(arr, data.length());
            int len = outputFile.fwrite(arr, 1, length);
            
            if (len != length) {
                cerr << "Error writing file " << file_path << 
                    "  errno=" << strerror(errno) << endl;
                exit(16);
            }

            outputFile.fclose();
        } else {
            cout << "Invalid message" << endl;
        }
    }
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