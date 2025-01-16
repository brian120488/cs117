
// IMPORTANT! WE INCLUDE THE IDL FILE AS IT DEFINES THE INTERFACES
// TO THE FUNCTIONS WE'RE IMPLEMENTING. THIS MAKES SURE THE
// CODE HERE ACTUALLY MATCHES THE REMOTED INTERFACE

#include "structwitharray.idl"

#include "rpcproxyhelper.h"

#include <cstdio>
#include <cstring>
#include <sstream>
#include "c150debug.h"

using namespace C150NETWORK;  // for all the comp150 utilities 
using namespace std;

int sqrt(s saw) {
    string arg0 = "sqrt";
    RPCPROXYSOCKET->write(arg0.c_str(), arg0.length() + 1); 

    int *m1 = saw.m1;
    for (int i = 0; i < 2; i++) {
        char arg0[sizeof(int)];
        memcpy(arg0, &m1[i], sizeof(int));
        RPCPROXYSOCKET->write(arg0, sizeof(int));
    }

    int (*m2)[3] = saw.m2;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            char arg1[sizeof(int)];
            memcpy(arg1, &m2[i][j], sizeof(int));
            RPCPROXYSOCKET->write(arg1, sizeof(int));
        }
    }

    int (*m3)[3][4] = saw.m3;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 4; k++) {
                char arg2[sizeof(int)];
                memcpy(arg2, &m3[i][j][k], sizeof(int));
                RPCPROXYSOCKET->write(arg2, sizeof(int));
            }
        }
    }

    char readBuffer[sizeof(int)];
    RPCPROXYSOCKET->read(readBuffer, sizeof(int));
    int result;
    memcpy(&result, readBuffer, sizeof(int));
    return result;
}