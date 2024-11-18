
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
    char readBuffer[5];  // to read magic value DONE + null

    string arg0 = "sqrt";
    RPCPROXYSOCKET->write(arg0.c_str(), arg0.length() + 1); 

    char* arg1 = reinterpret_cast<char*>(&saw);
    RPCPROXYSOCKET->write(arg1, sizeof(s)); 

    RPCPROXYSOCKET->read(readBuffer, sizeof(readBuffer)); // only legal response is DONE
    cout << readBuffer << endl;

    return stoi(readBuffer);
}