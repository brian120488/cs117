
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

string array_to_string(int arr[24]) {
    stringstream ss;
    ss << "[";
    for (int i = 0; i < 24; i++) {
        ss << arr[i];
        ss << " ";
    } 
    ss << "]";
    return ss.str();
}

int sqrt(int x[24], int y[24]) {
    char readBuffer[5];  // to read magic value DONE + null
    
    string n = "sqrt";
    RPCPROXYSOCKET->write(n.c_str(), n.length()+1); // write function name including null

    for (int i = 0; i < 24; i++) {
        std::ostringstream arg1;
        arg1 << x[i];
        cout << arg1.str() << endl;
        RPCPROXYSOCKET->write(arg1.str().c_str(), arg1.str().length()+1); 
    }

    // char* arg1 = reinterpret_cast<char*>(x);
    // cout << "LENGTH OF STRING: "<< strlen(arg1) << endl;
    // RPCPROXYSOCKET->write(arg1, 24 * 4); 
    // for (int i = 0; i < 24; i++) {
    //     cout << arg1[i] << endl;
    // }

    for (int i = 0; i < 24; i++) {
        std::ostringstream arg2;
        arg2 << y[i];
        cout << arg2.str() << endl;
        RPCPROXYSOCKET->write(arg2.str().c_str(), arg2.str().length()+1); 
    }


    // string message = "sqrt(" + array_to_string(x) + " " + array_to_string(y) + ")";
    // // string message = "sqrt(3, " + array_to_string(y) + ")";

    // //
    // // Send the Remote Call
    // //
    // //   c150debug->printf(C150RPCDEBUG,"arithmetic.proxy.cpp: " + message + " invoked");
    // RPCPROXYSOCKET->write(message.c_str(), message.length()+1); // write function name including null

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