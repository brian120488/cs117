
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

string array_to_string(int arr[3]) {
    stringstream ss;
    ss << "[";
    for (int i = 0; i < 3; i++) {
        ss << arr[i];
        ss << " ";
    } 
    ss << "]";
    return ss.str();
}

string array2d_to_string(int x[3][2]) {
    stringstream ss;
    ss << "[";
    for (int i = 0; i < 3; i++) {
        ss << "[";
        for (int j = 0; j < 2; j++) {
            ss << x[i][j];
            if (j < 1) ss << " ";  // Add comma between elements in the row
        }
        ss << "]";
        if (i < 2) ss << " ";  // Add comma between rows
    }
    ss << "]";
    return ss.str();
}

string array3d_to_string(int x[2][3][4]) {
    stringstream ss;
    ss << "[";
    for (int i = 0; i < 2; i++) {
        ss << "[";
        for (int j = 0; j < 3; j++) {
            ss << "[";
            for (int k = 0; k < 4; k++) {
                ss << x[i][j][k];
                if (k < 1) ss << " ";  // Add comma between elements in the innermost array
            }
            ss << "]";
            if (j < 2) ss << " ";  // Add comma between subarrays
        }
        ss << "]";
        if (i < 1) ss << " ";  // Add comma between rows of 2D arrays
    }
    ss << "]";
    return ss.str();
}


int sqrt(s saw) {
    char readBuffer[5];  // to read magic value DONE + null
    
    string n = "sqrt";
    RPCPROXYSOCKET->write(n.c_str(), n.length()+1); // write function name including null

    for (int i = 0; i < 2; i++) {
        std::ostringstream arg;
        arg << saw.m1[i];
        RPCPROXYSOCKET->write(arg.str().c_str(), arg.str().length()+1); 
    }

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            std::ostringstream arg;
            arg << saw.m2[i][j];
            RPCPROXYSOCKET->write(arg.str().c_str(), arg.str().length()+1); 
        }
    }

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 4; k++) {
                std::ostringstream arg;
                arg << saw.m3[i][j][k];
                RPCPROXYSOCKET->write(arg.str().c_str(), arg.str().length()+1); 
            }
        }
    }

    RPCPROXYSOCKET->read(readBuffer, sizeof(readBuffer)); // only legal response is DONE
    cout << readBuffer << endl;

    return stoi(readBuffer);
}