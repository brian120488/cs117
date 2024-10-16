// 
//            processing.h
//
//     Author: Brian Yang and Manuel Pena
//
//     Module that contains necessary functions to process data for
//     both fileclient.cpp and fileserver.cpp
//
//     
//

#ifndef HASHING_H
#define HASHING_H

#include <string>
#include <vector>

using namespace std;

string make_hash(const string& file_path);  // Function declaration
string make_data_hash(const string& data);  // Function declaration
vector<string> split(const string &str, char delimiter);
vector<string> split_with_limit(const string &str, char delimiter, int limit);

#endif