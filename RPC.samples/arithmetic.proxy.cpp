
// IMPORTANT! WE INCLUDE THE IDL FILE AS IT DEFINES THE INTERFACES
// TO THE FUNCTIONS WE'RE IMPLEMENTING. THIS MAKES SURE THE
// CODE HERE ACTUALLY MATCHES THE REMOTED INTERFACE

#include "arithmetic.idl"

#include "rpcproxyhelper.h"

#include <cstdio>
#include <cstring>
#include <sstream>
#include "c150debug.h"

using namespace C150NETWORK;  // for all the comp150 utilities 
using namespace std;

int add(int x, int y) {
    char readBuffer[5];  // to read magic value DONE + null

    string n = "add";
    
    std::ostringstream arg1;
    arg1 << reinterpret_cast<int*>(reinterpret_cast<char*>(x));

    char *arg2 = reinterpret_cast<char*>(y);
    
    RPCPROXYSOCKET->write(n.c_str(), n.length()+1); // write function name including null
    RPCPROXYSOCKET->write(arg1.str().c_str(), arg1.str().length()+1); 
    RPCPROXYSOCKET->write(arg2, strlen(arg2)+1); 

    RPCPROXYSOCKET->read(readBuffer, sizeof(readBuffer)); // only legal response is DONE
    cout << readBuffer << endl;

    //   if (strncmp(readBuffer,"DONE", sizeof(readBuffer))!=0) {
    //     throw C150Exception("arithmetic.proxy: " + message + " received invalid response from the server");
    //   }
    return stoi(readBuffer);
}


int subtract(int x, int y) {
  char readBuffer[5];
  string message = "subtract(" + to_string(x) + ", " + to_string(y) + ")";

//   c150debug->printf(C150RPCDEBUG,"arithmetic.proxy.cpp: " + message+ " invoked");
  RPCPROXYSOCKET->write(message.c_str(), message.length()+1); // write function name including null

//   c150debug->printf(C150RPCDEBUG,"arithmetic.proxy.cpp: " + message + " invocation sent, waiting for response");
  RPCPROXYSOCKET->read(readBuffer, sizeof(readBuffer)); // only legal response is DONE

  if (strncmp(readBuffer,"DONE", sizeof(readBuffer))!=0) {
    throw C150Exception("arithmetic.proxy: " + message + " received invalid response from the server");
  }
//   c150debug->printf(C150RPCDEBUG,"arithmetic.proxy.cpp: " + message + " successful return from remote call");

    return 0;
}


int multiply(int x, int y) {
  char readBuffer[5];
  string message = "multiply(" + to_string(x) + ", " + to_string(y) + ")";

  // c150debug->printf(C150RPCDEBUG,"arithmetic.proxy.cpp: " + message + " invoked");
  RPCPROXYSOCKET->write(message.c_str(), message.length()+1);

  // c150debug->printf(C150RPCDEBUG,"arithmetic.proxy.cpp: " + message + " invocation sent, waiting for response");
  RPCPROXYSOCKET->read(readBuffer, sizeof(readBuffer));

  if (strncmp(readBuffer,"DONE", sizeof(readBuffer))!=0) {
    throw C150Exception("arithmetic.proxy: " + message + " received invalid response from the server");
  }
  // c150debug->printf(C150RPCDEBUG,"arithmetic.proxy.cpp: " + message + " successful return from remote call");
  return 0;
}

int divide(int x, int y) {
  char readBuffer[5];
  string message = "divide(" + to_string(x) + ", " + to_string(y) + ")";

  // c150debug->printf(C150RPCDEBUG,"arithmetic.proxy.cpp: " + message + " invoked");
  RPCPROXYSOCKET->write(message.c_str(), message.length()+1);

  // c150debug->printf(C150RPCDEBUG,"arithmetic.proxy.cpp: " + message + " invocation sent, waiting for response");
  RPCPROXYSOCKET->read(readBuffer, sizeof(readBuffer));

  if (strncmp(readBuffer,"DONE", sizeof(readBuffer))!=0) {
    throw C150Exception("arithmetic.proxy: " + message + " received invalid response from the server");
  }
  // c150debug->printf(C150RPCDEBUG,"arithmetic.proxy.cpp: " + message + " successful return from remote call");

    return 0;
}
