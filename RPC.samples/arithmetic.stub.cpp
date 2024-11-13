// --------------------------------------------------------------
//
//                        simplefunction.stub.cpp
//
//        Author: Noah Mendelsohn         
//   
//       This is a hand-crafted demonstration stub.
//
//       It implements a very simple protocol that works only
//       for functions with no arguments and void return. Invocation
//       requests are just the null terminated function name; 
//       Responses are the null terminated words DONE or BAD.
//       You'll have to do something much more sophisticated
//       for routines that accept and return values. (And you might
//       not want to use nulls as delimiters in your protocol, since
//       some of the values you send might contain them!)
//
//       For your project, your "rpcgen" program will generate
//       stubs like this one automatically from the idl
//       file. Note that this stub also #includes the 
//       simplefunctions.idl file. Of course, your rpcgen
//       program will change that, as well as the number
//       of functions generated. More importantly, it will
//       generate code to handle function arguments and
//       return values.
//
//       You can structure your stub however you like. This
//       one relies on the container to loop calling 
//       dispatchFunction() until eof is reached, but you
//       can do it other ways. In general, there needs to
//       be some place where you read the stream to see
//       which function to call; how you do that is up to you.
//
//       Copyright: 2012 Noah Mendelsohn
//
// --------------------------------------------------------------

// IMPORTANT! WE INCLUDE THE IDL FILE AS IT DEFINES THE INTERFACES
// TO THE FUNCTIONS WE'RE IMPLEMENTING. THIS MAKES SURE THE
// CODE HERE ACTUALLY MATCHES THE REMOTED INTERFACE

#include "arithmetic.idl"

#include "rpcstubhelper.h"

#include <cstdio>
#include <cstring>
#include "c150debug.h"

using namespace C150NETWORK;  // for all the comp150 utilities 

struct Function {
    string name;
    vector<string> args;
};

Function *getFunctionFromStream(char *buffer, unsigned int bufSize);

// ======================================================================
//                             STUBS
//
//    Each of these is generated to match the signature of the 
//    function for which it is a stub. The names are prefixed with "__"
//    to keep them distinct from the actual routines, which we call!
//
//    Note that when functions take arguments, these are the routines
//    that will read them from the input stream. These routines should
//    be generated dynamically from your rpcgen program (and so should the
//    code above).
//
// ======================================================================
  


void __add(int x, int y) {

  //
  // Time to actually call the function 
  //
  c150debug->printf(C150RPCDEBUG,"simplefunction.stub.cpp: invoking func1()");
  cout << "Adding...\n";
  int output = add(x, y);

  //
  // Send the response to the client
  //
  // If func1 returned something other than void, this is
  // where we'd send the return value back.
  //
  // c150debug->printf(C150RPCDEBUG,"simplefunction.stub.cpp: returned from  func1() -- responding to client");
  // RPCSTUBSOCKET->write(doneBuffer, strlen(doneBuffer)+1);

  char outputBuffer[50]; // Buffer to hold the output string
  snprintf(outputBuffer, sizeof(outputBuffer), "%d", output); // Convert output to string

  c150debug->printf(C150RPCDEBUG,"simplefunction.stub.cpp: returned from  func3() -- responding to client");
  RPCSTUBSOCKET->write(outputBuffer, strlen(outputBuffer)+1);
}

void __subtract(int x, int y) {
  c150debug->printf(C150RPCDEBUG,"simplefunction.stub.cpp: invoking func2()");
  int output = subtract(x, y);

  char outputBuffer[50]; // Buffer to hold the output string
  snprintf(outputBuffer, sizeof(outputBuffer), "%d", output); // Convert output to string

  c150debug->printf(C150RPCDEBUG,"simplefunction.stub.cpp: returned from  func3() -- responding to client");
  RPCSTUBSOCKET->write(outputBuffer, strlen(outputBuffer)+1);
}

void __multiply(int x, int y) {
  c150debug->printf(C150RPCDEBUG,"simplefunction.stub.cpp: invoking func3()");
  int output = multiply(x, y);

  char outputBuffer[50]; // Buffer to hold the output string
  snprintf(outputBuffer, sizeof(outputBuffer), "%d", output); // Convert output to string

  c150debug->printf(C150RPCDEBUG,"simplefunction.stub.cpp: returned from  func3() -- responding to client");
  RPCSTUBSOCKET->write(outputBuffer, strlen(outputBuffer)+1);
}

void __divide(int x, int y) {

  c150debug->printf(C150RPCDEBUG,"simplefunction.stub.cpp: invoking divide()");
  int output = divide(x, y);
  char outputBuffer[50]; // Buffer to hold the output string
  snprintf(outputBuffer, sizeof(outputBuffer), "%d", output); // Convert output to string

  c150debug->printf(C150RPCDEBUG,"simplefunction.stub.cpp: returned from  func3() -- responding to client");
  RPCSTUBSOCKET->write(outputBuffer, strlen(outputBuffer)+1);
}


//
//     __badFunction
//
//   Pseudo-stub for missing functions.
//

// void __badFunction(char *functionName) {
//   char doneBuffer[5] = "BAD";  // to write magic value DONE + null


//   //
//   // Send the response to the client indicating bad function
//   //

//   c150debug->printf(C150RPCDEBUG,"simplefunction.stub.cpp: received call for nonexistent function %s()",functionName);
//   RPCSTUBSOCKET->write(doneBuffer, strlen(doneBuffer)+1);
// }



// ======================================================================
//
//                        COMMON SUPPORT FUNCTIONS
//
// ======================================================================



//
//                         dispatchFunction()
//
//   Called when we're ready to read a new invocation request from the stream
//
void dispatchFunction() {

  cout << "In dispatch\n";
  char functionNameBuffer[50];

  //
  // Read the function name from the stream -- note
  // REPLACE THIS WITH YOUR OWN LOGIC DEPENDING ON THE 
  // WIRE FORMAT YOU USE
  //
  Function *fp = getFunctionFromStream(functionNameBuffer,sizeof(functionNameBuffer));
  if (fp == NULL) return;
  Function f = *fp;

  //
  // We've read the function name, call the stub for the right one
  // The stub will invoke the function and send response.
  //
  
    int x = stoi(f.args[0]);
    int y = stoi(f.args[1]);
    cout << f.name << " " << x << " " << y << endl;
    if (!RPCSTUBSOCKET-> eof()) {
        if (f.name == "add")
        __add(x, y);
        else   if (f.name == "subtract")
        __subtract(x, y);
        else   if (f.name == "multiply")
        __multiply(x, y);
        else   if (f.name == "divide")
        __divide(x, y);
        // else
        //   __badFunction(func.name.c_str());
    }
}

 
//
//                   getFunctionNamefromStream
//
//   Helper routine to read function name from the stream. 
//   Note that this code is the same for all stubs, so can be generated
//   as boilerplate.
//
//   Important: this routine must leave the sock open but at EOF
//   when eof is read from client.
//

Function *getFunctionFromStream(char *buffer, unsigned int bufSize) {  
    //
    // Read a message from the stream
    // -1 in size below is to leave room for null
    //
    Function *f = new Function;
    bool readnull = false;
    ssize_t readlen;             // amount of data read from socket
    char *bufp = buffer;   // next char to read
    for (unsigned i=0; i < bufSize; i++) {
        readlen = RPCSTUBSOCKET-> read(bufp, 1);  // read a byte
        // check for eof or error
        if (readlen == 0) return NULL;
        if (*bufp == '(') {
            f->name = string(buffer);
            f->name.pop_back();
            break;
        }
        // check for null and bump buffer pointer
        if (*bufp++ == '\0') {
            readnull = true;
            break;
        }
    }

	while (!readnull) {
        buffer = bufp;

		for (unsigned i=0; i < bufSize; i++) {
			readlen = RPCSTUBSOCKET-> read(bufp, 1);
			if (readlen == 0) {
                readnull = true;
                break;
            }
            else if (*bufp == ' ') {
                continue;
            }
			else if (*bufp == ',') {
                string arg = string(buffer);
                arg.pop_back();
				f->args.push_back(arg);
				break;
			} else if (*bufp == ')') {
                string arg = string(buffer);
                arg.pop_back();
				f->args.push_back(arg);
				break;
			}
			if (*bufp++ == '\0') {
				readnull = true;
				break;
			}
		}
    }

    cout << "ARG1: " << f->args[0] << endl;
    cout << "ARG2: " << f->args[1] << endl;

  
  //
  // With TCP streams, we should never get a 0 length read
  // except with timeouts (which we're not setting in pingstreamserver)
  // or EOF
  //
  if (readlen == 0) {
    c150debug->printf(C150RPCDEBUG,"simplefunction.stub: read zero length message, checking EOF");
    if (RPCSTUBSOCKET-> eof()) {
      c150debug->printf(C150RPCDEBUG,"simplefunction.stub: EOF signaled on input");

    } else {
      throw C150Exception("simplefunction.stub: unexpected zero length read without eof");
    }
  }

  //
  // If we didn't get a null, input message was poorly formatted
  //
  else if (!readnull) 
    throw C150Exception("simplefunction.stub: method name not null terminated or too long");

  
  //
  // Note that eof may be set here for our caller to check
  //
    cout << "Returning func\n";
    return f;
}


