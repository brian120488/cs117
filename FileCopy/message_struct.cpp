#include <iostream>
#include <cstring>
#include <string>

using namespace std;

struct Message {
    string command;
    // string file_name;
    // int byte_offset;
    // string data;
    string[] arguments;

};


int main() {
    struct Message myData = {"command","file_name", 1, "data"};

    // Serialize the struct
    char* buffer = new char[sizeof(struct Message)];
    memcpy(buffer, &myData, sizeof(struct Message));

    // Deserialize the struct
    Message* recoveredData = reinterpret_cast<Message*>(buffer);

    cout << "Command: " << recoveredData->command << endl;
    cout << "Data: " << recoveredData->data << endl;

    delete[] buffer;

    return 0;
}