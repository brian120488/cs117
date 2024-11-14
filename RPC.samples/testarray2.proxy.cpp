
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


int sqrt(int x[3], int y[3][2], int z[3][2]) {
    char readBuffer[5];  // to read magic value DONE + null
    string message = "sqrt(" + array_to_string(x) + " " + array2d_to_string(y) + " " + array2d_to_string(z) + ")";
    // string message = "sqrt(3, " + array_to_string(y) + ")";

    //
    // Send the Remote Call
    //
    //   c150debug->printf(C150RPCDEBUG,"arithmetic.proxy.cpp: " + message + " invoked");
    RPCPROXYSOCKET->write(message.c_str(), message.length()+1); // write function name including null

    //
    // Read the response
    //
    //   c150debug->printf(C150RPCDEBUG,"arithmetic.proxy.cpp: " + message + " invocation sent, waiting for response");
    RPCPROXYSOCKET->read(readBuffer, sizeof(readBuffer)); // only legal response is DONE
    cout << readBuffer << endl;
  //
  // Check the response
  //
//   if (strncmp(readBuffer,"DONE", sizeof(readBuffer))!=0) {
//     throw C150Exception("arithmetic.proxy: " + message + " received invalid response from the server");
//   }
//   c150debug->printf(C150RPCDEBUG,"arithmetic.proxy.cpp: " + message + " successful return from remote call");

    return stoi(readBuffer);
}