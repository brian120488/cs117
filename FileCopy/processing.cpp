// 
//            processing.cpp
//
//     Author: Brian Yang and Manuel Pena
//
//     Module that contains the implementation of necessary functions to 
//     process data for both fileclient.cpp and fileserver.cpp
//
//     
//

#include "processing.h"
#include <fstream>
#include <sstream>
#include <openssl/sha.h>
#include <fstream>
#include <cstdlib> 
#include <sstream> 
#include <iomanip> 
#include <vector>
#include "c150nastyfile.h"
#include <cstdio>
#include <filesystem>
#include <csignal>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                          make_hash
//
//           Makes a hash key for a file given a file path.
//        
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
string make_hash(const string& file_path) {
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

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                        make_data_hash
//
//            Makes hash for data, used for packets.
//        
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
string make_data_hash(const string& data) {
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
//                 Split a string into tokens
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


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                      split_with_limit
//
//        Split a string into tokens using a delimiter
//        
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
vector<string> split_with_limit(const string &str, char delimiter, int limit = -1) {
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