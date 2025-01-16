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

#include "structwitharray.idl"

#include "rpcstubhelper.h"

#include <cstdio>
#include <cstring>
#include "c150debug.h"
#include <stack>

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
  


void __sqrt(s saw) {
    int output = sqrt(saw);
    
    char outputBuffer[sizeof(int)];
    memcpy(outputBuffer, &output, sizeof(int));
    RPCSTUBSOCKET->write(outputBuffer, sizeof(int));
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


void dispatchFunction() {
    char buffer[50];
    getFunctionNameFromStream(buffer, sizeof(buffer));
    string f_name(buffer);

    if (!RPCSTUBSOCKET-> eof()) {
        if (f_name == "sqrt") { 
            int m1[2];
            for (int i = 0; i < 2; i++) {
                char buffer0[sizeof(int)];
                RPCSTUBSOCKET->read(buffer0, sizeof(int));
                int ele;
                memcpy(&ele, buffer0, sizeof(int));
                m1[i] = ele;
            }

            int m2[2][3];
            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 3; j++) {
                    char buffer1[sizeof(int)];
                    RPCSTUBSOCKET->read(buffer1, sizeof(int));
                    int ele;
                    memcpy(&ele, buffer1, sizeof(int));
                    m2[i][j] = ele;
                }
            }

            int m3[2][3][4];
            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 3; j++) {
                    for (int k = 0; k < 4; k++) {
                        char buffer2[sizeof(int)];
                        RPCSTUBSOCKET->read(buffer2, sizeof(int));
                        int ele;
                        memcpy(&ele, buffer2, sizeof(int));
                        m3[i][j][k] = ele;
                    }
                }
            }
            
            s saw;
            memcpy(saw.m1, m1, sizeof(m1));
            memcpy(saw.m2, m2, sizeof(m2));
            memcpy(saw.m3, m3, sizeof(m3));

            __sqrt(saw);
        }
        // else
        //   __badFunction(func.name.c_str());
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

void getDataFromStream(char *buffer, unsigned int bufSize) {
    char *bufp = buffer;        
    ssize_t readlen;
    for (unsigned i = 0; i < bufSize; i++) {
        readlen = RPCSTUBSOCKET->read(bufp, 1);
        if (readlen == 0) break;
        bufp++;
    }
}


