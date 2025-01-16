
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

    for (int i = 0; i < 3; i++) {
        write_int(x[i]);
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            write_int(y[i][j]);
        }
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                write_int(z[i][j][k]);
            }
        }
    }

    char readBuffer[sizeof(int)];
    RPCPROXYSOCKET->read(readBuffer, sizeof(int));
    int result;
    memcpy(&result, readBuffer, sizeof(int));
    return result;
}