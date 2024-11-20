#include "structs.idl"
#include "rpcproxyhelper.h"

using namespace std;
using namespace C150NETWORK;

Person findPerson(ThreePeople ps) {
    string arg0 = "findPerson";
    RPCPROXYSOCKET->write(arg0.c_str(), arg0.length() + 1); 

    char* arg1 = reinterpret_cast<char*>(&ps);
    RPCPROXYSOCKET->write(arg1, sizeof(ThreePeople)); 

    char readBuffer[sizeof(Person)];
    RPCPROXYSOCKET->read(readBuffer, sizeof(readBuffer));
    return *reinterpret_cast<Person*>(readBuffer);
}

int area(rectangle r) {
    string arg0 = "area";
    RPCPROXYSOCKET->write(arg0.c_str(), arg0.length() + 1); 

    char* arg1 = reinterpret_cast<char*>(&r);
    RPCPROXYSOCKET->write(arg1, sizeof(rectangle)); 

    char readBuffer[sizeof(int)];
    RPCPROXYSOCKET->read(readBuffer, sizeof(readBuffer));
    return *reinterpret_cast<int*>(readBuffer);
}

void getDataFromStream(char *buffer, unsigned int bufSize) {
    char *bufp = buffer;        
    ssize_t readlen;
    for (unsigned i = 0; i < bufSize; i++) {
        readlen = RPCPROXYSOCKET->read(bufp, 1);
        if (readlen == 0) break;
        bufp++;
    }
}