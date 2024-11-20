#include "arithmetic.idl"
#include "rpcproxyhelper.h"

using namespace C150NETWORK;  // for all the comp150 utilities 
using namespace std;

int add(int x, int y) {
    string arg0 = "add";
    RPCPROXYSOCKET->write(arg0.c_str(), arg0.length()+1);

    char *arg1 = reinterpret_cast<char*>(&x);
    RPCPROXYSOCKET->write(arg1, sizeof(int)); 

    char *arg2 = reinterpret_cast<char*>(&y);
    RPCPROXYSOCKET->write(arg2, sizeof(int)); 

    char readBuffer[sizeof(int)];
    RPCPROXYSOCKET->read(readBuffer, sizeof(readBuffer));
    return *reinterpret_cast<int*>(readBuffer);
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
