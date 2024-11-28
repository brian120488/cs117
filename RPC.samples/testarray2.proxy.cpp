
// IMPORTANT! WE INCLUDE THE IDL FILE AS IT DEFINES THE INTERFACES
// TO THE FUNCTIONS WE'RE IMPLEMENTING. THIS MAKES SURE THE
// CODE HERE ACTUALLY MATCHES THE REMOTED INTERFACE

#include "testarray2.idl"

#include "rpcproxyhelper.h"

#include <cstdio>
#include <cstring>

using namespace C150NETWORK;  // for all the comp150 utilities 
using namespace std;

int sqrt(int x[3], int y[3][2], int z[3][2][2]) {
    string name = "sqrt";
    RPCPROXYSOCKET->write(name.c_str(), name.length() + 1); 

    char* arg0 = reinterpret_cast<char*>(x);
    RPCPROXYSOCKET->write(arg0, sizeof(int) * 3); 

    char* arg1 = reinterpret_cast<char*>(y);
    RPCPROXYSOCKET->write(arg1, sizeof(int) * 3 * 2); 

    char* arg2 = reinterpret_cast<char*>(z);
    RPCPROXYSOCKET->write(arg2, sizeof(int) * 3 * 2 * 2); 

    char readBuffer[sizeof(int)];
    RPCPROXYSOCKET->read(readBuffer, sizeof(int)); // only legal response is DONE
    return *reinterpret_cast<int*>(readBuffer);
}