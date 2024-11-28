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

#include "testarray2.idl"

#include "rpcstubhelper.h"

#include <cstdio>
#include <cstring>

using namespace C150NETWORK;  // for all the comp150 utilities 


void getFunctionNameFromStream(char *buffer, unsigned int bufSize);
void getDataFromStream(char *buffer, unsigned int bufSize);

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
  


void __sqrt(int x[3], int y[3][2], int z[3][2][2]) {
    int output = sqrt(x, y, z);
    char *outputBuffer = reinterpret_cast<char*>(&output);
    RPCSTUBSOCKET->write(outputBuffer, sizeof(int));
}


void dispatchFunction() {
    char buffer[50];
    getFunctionNameFromStream(buffer, sizeof(buffer));
    string f_name(buffer);

    if (!RPCSTUBSOCKET-> eof()) {
        if (f_name == "sqrt") {
            char buffer0[sizeof(int) * 3];
            RPCSTUBSOCKET->read(buffer0, sizeof(int) * 3);
            int *x = reinterpret_cast<int(*)>(buffer0);

            char buffer1[sizeof(int) * 3 * 2];
            RPCSTUBSOCKET->read(buffer1, sizeof(int) * 3 * 2);
            int (*y)[2] = reinterpret_cast<int(*)[2]>(buffer1);

            char buffer2[sizeof(int) * 3 * 2 * 2];
            RPCSTUBSOCKET->read(buffer2, sizeof(int) * 3 * 2 * 2);
            int (*z)[2][2] = reinterpret_cast<int(*)[2][2]>(buffer2);

            __sqrt(x, y, z);
        }
    }
}


void getFunctionNameFromStream(char *buffer, unsigned bufSize) {
    char *bufp = buffer;     
    ssize_t readlen;
    for (unsigned i = 0; i < bufSize; i++) {
        readlen = RPCSTUBSOCKET->read(bufp, 1);
        if (readlen == 0) break;
        if (*bufp++ == '\0') break;
    }
}
