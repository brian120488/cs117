#include <string>
using namespace std;

#include "structs.idl"
#include "rpcstubhelper.h"

using namespace C150NETWORK;  // for all the comp150 utilities 


void getFunctionNameFromStream(char *buffer, unsigned bufSize) {
    char *bufp = buffer;     
    ssize_t readlen;  
    for (unsigned i = 0; i < bufSize; i++) {
        readlen = RPCSTUBSOCKET->read(bufp, 1);
        if (readlen == 0) break;
        if (*bufp++ == '\0') break;
    }
}

void write_int(int n) {
    char buffer[4];
    memcpy(buffer, &n, 4);
    RPCSTUBSOCKET->write(buffer, 4);
}

int read_int() {
    char buffer[4];
    RPCSTUBSOCKET->read(buffer, 4);
    int n;
    memcpy(&n, buffer, 4);
    return n;
}

void write_float(float n) {
    char buffer[4];
    memcpy(buffer, &n, 4);
    RPCSTUBSOCKET->write(buffer, 4);
}

int read_float() {
    char buffer[4];
    RPCSTUBSOCKET->read(buffer, 4);
    float n;
    memcpy(&n, buffer, 4);
    return n;
}

void write_string(string s) {
    write_int(s.length());
    RPCSTUBSOCKET->write(s.c_str(), s.length() + 1);
}

string read_string() {
    int length = read_int();
    // cout << length << endl;
    char buffer[length + 1]; 
    RPCSTUBSOCKET->read(buffer, length + 1); 
    string s(buffer);
    return s;
}

void __findPerson(ThreePeople ps) {
    Person output = findPerson(ps);

    write_string(output.firstname);
    write_string(output.lastname);
    write_int(output.age);
}

void __area(rectangle r) {
  cout << "Finding...\n";
  int output = area(r);
  cout << "OUTPUT: " << output << endl;

  char *outputBuffer = reinterpret_cast<char*>(&output);
  RPCSTUBSOCKET->write(outputBuffer, sizeof(int));
}


void dispatchFunction() {
    cout << "In dispatch\n";
    char buffer[50];
    getFunctionNameFromStream(buffer,sizeof(buffer));
    string f_name(buffer);

    if (!RPCSTUBSOCKET-> eof()) {
        if (f_name == "findPerson") {
            Person p1;
            p1.firstname = read_string();
            p1.lastname = read_string();
            p1.age = read_int();

            Person p2;
            p2.firstname = read_string();
            p2.lastname = read_string();
            p2.age = read_int();

            Person p3;
            p3.firstname = read_string();
            p3.lastname = read_string();
            p3.age = read_int();

            ThreePeople ps;
            ps.p1 = p1;
            ps.p2 = p2;
            ps.p3 = p3;
            __findPerson(ps);
        } 
        else if (f_name == "area") {
            cout << "in area" << endl;
            char buffer1[sizeof(rectangle)];
            RPCSTUBSOCKET->read(buffer1, sizeof(rectangle));
            rectangle r = *reinterpret_cast<rectangle*>(buffer1);
            cout << r.x << " " << r.y << endl;
            __area(r);
        } else {
            cout << "actual name " << f_name << endl;
        }
    }
}
