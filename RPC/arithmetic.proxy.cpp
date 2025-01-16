// Proxy file generated for arithmetic

#include <string>
using namespace std;  

#include "arithmetic.idl"
#include "rpcproxyhelper.h"

using namespace C150NETWORK;

void getFunctionNameFromStream(char *buffer, unsigned bufSize) {
    char *bufp = buffer;     
    ssize_t readlen;
    for (unsigned i = 0; i < bufSize; i++) {
        readlen = RPCPROXYSOCKET->read(bufp, 1);
		cout << i << " " << bufp << endl;
        if (readlen == 0) break;
        if (*bufp++ == '\0') break;
    }
}

int add(int x, int y) {
    string name = "add";
    RPCPROXYSOCKET->write(name.c_str(), name.length() + 1);

    char arg0[sizeof(int)];
    memcpy(arg0, &x, sizeof(int));
    RPCPROXYSOCKET->write(arg0, sizeof(int));

    char arg1[sizeof(int)];
    memcpy(arg1, &y, sizeof(int));
    RPCPROXYSOCKET->write(arg1, sizeof(int));

    char readBuffer[sizeof(int)];
    RPCPROXYSOCKET->read(readBuffer, sizeof(int));
    
    int result;
    memcpy(&result, readBuffer, sizeof(int));
    return result;
}

int divide(int x, int y) {
    string name = "divide";
    RPCPROXYSOCKET->write(name.c_str(), name.length() + 1);

    char* arg0 = reinterpret_cast<char*>(&x);
    RPCPROXYSOCKET->write(arg0, sizeof(int));

    char* arg1 = reinterpret_cast<char*>(&y);
    RPCPROXYSOCKET->write(arg1, sizeof(int));

    char readBuffer[sizeof(int)];
    RPCPROXYSOCKET->read(readBuffer, sizeof(int));
    return *reinterpret_cast<int*>(readBuffer);
}

int multiply(int x, int y) {
    string name = "multiply";
    RPCPROXYSOCKET->write(name.c_str(), name.length() + 1);

    char* arg0 = reinterpret_cast<char*>(&x);
    RPCPROXYSOCKET->write(arg0, sizeof(int));

    char* arg1 = reinterpret_cast<char*>(&y);
    RPCPROXYSOCKET->write(arg1, sizeof(int));

    char readBuffer[sizeof(int)];
    RPCPROXYSOCKET->read(readBuffer, sizeof(int));
    return *reinterpret_cast<int*>(readBuffer);
}

int subtract(int x, int y) {
    string name = "subtract";
    RPCPROXYSOCKET->write(name.c_str(), name.length() + 1);

    char* arg0 = reinterpret_cast<char*>(&x);
    RPCPROXYSOCKET->write(arg0, sizeof(int));

    char* arg1 = reinterpret_cast<char*>(&y);
    RPCPROXYSOCKET->write(arg1, sizeof(int));

    char readBuffer[sizeof(int)];
    RPCPROXYSOCKET->read(readBuffer, sizeof(int));
    return *reinterpret_cast<int*>(readBuffer);
}

string reply(string s) {
    string name = "reply";
    RPCPROXYSOCKET->write(name.c_str(), name.length() + 1);
    
    RPCPROXYSOCKET->write(s.c_str(), s.length() + 1);
    // char* arg0 = reinterpret_cast<char*>(&s);
    // cout << "cast: " << arg0 << endl;
    // RPCPROXYSOCKET->write(arg0, sizeof(string));

    char readBuffer[50];
    getFunctionNameFromStream(readBuffer, sizeof(readBuffer));
    // string ret(readBuffer);
    return readBuffer;
}
