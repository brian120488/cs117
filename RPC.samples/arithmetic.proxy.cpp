
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
  string message = "add(" + to_string(x) + ", " + to_string(y) + ")";

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
