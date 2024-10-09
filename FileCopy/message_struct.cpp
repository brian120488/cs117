#include <iostream>
#include <cstring>
#include <string>

using namespace std;

struct Message {
    string command;
    string file_name;
    int byte_offset;
    char data[256];
    string hash;
    // string[] arguments;

};


int main() {
    struct Message myData = {"command", "hi", 1, "data", "hash"};
    
    cout << sizeof(struct Message) << endl;
    // Serialize the struct
    char* buffer = new char[sizeof(struct Message)];
    memcpy(buffer, &myData, sizeof(struct Message));

    // Deserialize the struct
    struct Message* recoveredData = reinterpret_cast<struct Message*>(buffer);

    cout << "Command: " << recoveredData->command << endl;
    cout << "Data: " << recoveredData->hash << endl;

    delete[] buffer;

    return 0;
}