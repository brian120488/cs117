
// IMPORTANT! WE INCLUDE THE IDL FILE AS IT DEFINES THE INTERFACES
// TO THE FUNCTIONS WE'RE IMPLEMENTING. THIS MAKES SURE THE
// CODE HERE ACTUALLY MATCHES THE REMOTED INTERFACE

#include "testarray1.idl"

#include "rpcproxyhelper.h"

#include <cstdio>
#include <cstring>
#include <sstream>
#include "c150debug.h"

using namespace C150NETWORK;  // for all the comp150 utilities 
using namespace std;

int sqrt(int x[24], int y[24]) {
    char readBuffer[5];  // to read magic value DONE + null

    string arg0 = "sqrt";
    RPCPROXYSOCKET->write(arg0.c_str(), arg0.length() + 1); 

    char* arg1 = reinterpret_cast<char*>(x);
    RPCPROXYSOCKET->write(arg1, sizeof(int) * 24); 

    char* arg2 = reinterpret_cast<char*>(y);
    RPCPROXYSOCKET->write(arg2, sizeof(int) * 24); 

    RPCPROXYSOCKET->read(readBuffer, sizeof(readBuffer));
    cout << readBuffer << endl;

    return stoi(readBuffer);
}