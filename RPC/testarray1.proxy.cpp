
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
    string arg0 = "sqrt";
    RPCPROXYSOCKET->write(arg0.c_str(), arg0.length() + 1); 

    for (int i = 0; i < 24; i++) {
        char arg0[sizeof(int)];
        memcpy(arg0, &x[i], sizeof(int));
        RPCPROXYSOCKET->write(arg0, sizeof(int));
    }

    for (int i = 0; i < 24; i++) {
        char arg1[sizeof(int)];
        memcpy(arg1, &y[i], sizeof(int));
        RPCPROXYSOCKET->write(arg1, sizeof(int));
    }

    char readBuffer[sizeof(int)];
    RPCPROXYSOCKET->read(readBuffer, sizeof(int));
    
    int result;
    memcpy(&result, readBuffer, sizeof(int));
    return result;
}