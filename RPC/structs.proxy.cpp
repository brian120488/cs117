#include <string>
using namespace std;

#include "structs.idl"
#include "rpcproxyhelper.h"

using namespace C150NETWORK;


void getFunctionNameFromStream(char *buffer, unsigned bufSize) {
    char *bufp = buffer;     
    ssize_t readlen;  
    for (unsigned i = 0; i < bufSize; i++) {
        readlen = RPCPROXYSOCKET->read(bufp, 1);
        if (readlen == 0) break;
        if (*bufp++ == '\0') break;
    }
}

void write_int(int n) {
    char buffer[4];
    memcpy(buffer, &n, 4);
    RPCPROXYSOCKET->write(buffer, 4);
}

int read_int() {
    char buffer[4];
    RPCPROXYSOCKET->read(buffer, 4);
    int n;
    memcpy(&n, buffer, 4);
    return n;
}

void write_float(float n) {
    char buffer[4];
    memcpy(buffer, &n, 4);
    RPCPROXYSOCKET->write(buffer, 4);
}

int read_float() {
    char buffer[4];
    RPCPROXYSOCKET->read(buffer, 4);
    float n;
    memcpy(&n, buffer, 4);
    return n;
}

void write_string(string s) {
    write_int(s.length());
    RPCPROXYSOCKET->write(s.c_str(), s.length() + 1);
}

string read_string() {
    int length = read_int();
    char buffer[length + 1]; 
    RPCPROXYSOCKET->read(buffer, length + 1); 
    string s(buffer);
    return s;
}

Person findPerson(ThreePeople ps) {
    string arg0 = "findPerson";
    RPCPROXYSOCKET->write(arg0.c_str(), arg0.length() + 1);

    Person p1 = ps.p1;
    write_string(p1.firstname);
    write_string(p1.lastname);
    write_int(p1.age);

    Person p2 = ps.p2;
    write_string(p2.firstname);
    write_string(p2.lastname);
    write_int(p2.age);


    Person p3 = ps.p3;
    write_string(p3.firstname);
    write_string(p3.lastname);
    write_int(p3.age);

    Person result;
    result.firstname = read_string();
    result.lastname = read_string();
    result.age = read_int();
    return result;
}

int area(rectangle r) {
    string arg0 = "area";
    RPCPROXYSOCKET->write(arg0.c_str(), arg0.length() + 1); 

    char* arg2 = reinterpret_cast<char*>(&r);
    RPCPROXYSOCKET->write(arg2, sizeof(rectangle)); 

    char readBuffer[sizeof(int)];
    RPCPROXYSOCKET->read(readBuffer, sizeof(int));
    return *reinterpret_cast<int*>(readBuffer);
}