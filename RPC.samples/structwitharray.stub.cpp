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
  


void __sqrt(s swa) {

  //
  // Time to actually call the function 
  //
  c150debug->printf(C150RPCDEBUG,"testarray1.stub.cpp: invoking sqrt()");
  cout << "Sqrting...\n";
  int output = sqrt(swa);
  cout << "OUTPUT: " << output << endl;

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

  c150debug->printf(C150RPCDEBUG,"testarray1.stub.cpp: returned from sqrt() -- responding to client");
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

int* string_to_array(string s) {
    // remove []
    cout << "STRING: " << s << endl;
    s.erase(0, 1);
    s.pop_back();

    int *arr = new int[2];
    stringstream ss(s);
    string num;
    int index = 0;
    while (getline(ss, num, ' ')) {
        if (index >= 2) break;  // Ensure no overflow in the array
        arr[index++] = stoi(num);
    }
    return arr;
}


int (*string_to_array2d(string s))[3] {
    // remove []
    cout << "STRING: " << s << endl;
    s.erase(0, 1);
    s.pop_back();

    int (*arr)[3] = new int[2][3];

    stringstream ss(s);
    string num;
    int row = 0, col = 0;

    while (getline(ss, num, ' ')) {
        // Remove any remaining brackets or spaces
        num.erase(remove(num.begin(), num.end(), '['), num.end());
        num.erase(remove(num.begin(), num.end(), ']'), num.end());
        num.erase(remove(num.begin(), num.end(), ' '), num.end());
        if (num == "") continue;
        cout << "NUM: " << num << endl;
        // Convert string to integer and assign to 2D array
        arr[row][col] = stoi(num);

        // Move to next column or row
        col++;
        if (col == 3) {  // Reset column and move to next row
            col = 0;
            row++;
        }
        if (row == 2) break;  // Stop if we have filled the 2x3 array
    }
    return arr;
}

int (*string_to_array3d(string s))[3][4] {
    // Remove the outermost brackets
    s.erase(0, 1);
    s.pop_back();

    int (*arr)[3][4] = new int[2][3][4];

    stringstream ss(s);
    string num;
    int layer = 0, row = 0, col = 0;

    while (getline(ss, num, ' ')) {
        // Remove any remaining brackets or spaces
        num.erase(remove(num.begin(), num.end(), '['), num.end());
        num.erase(remove(num.begin(), num.end(), ']'), num.end());
        num.erase(remove(num.begin(), num.end(), ' '), num.end());
        if (num == "") continue;
        
        // Convert string to integer and assign to 3D array
        arr[layer][row][col] = stoi(num);

        // Move to next column, row, or layer as needed
        col++;
        if (col == 4) {   // Move to next row after filling columns
            col = 0;
            row++;
        }
        if (row == 3) {   // Move to next layer after filling rows
            row = 0;
            layer++;
        }
        if (layer == 2) break;  // Stop if we have filled the 2x3x4 array
    }
    return arr;
}

//
//                         dispatchFunction()
//
//   Called when we're ready to read a new invocation request from the stream
//
void dispatchFunction() {

    cout << "In dispatch\n";
    char buffer[50];
    getFunctionNameFromStream(buffer,sizeof(buffer));
    char *f_name = buffer;
    cout <<  f_name << endl;
    

    char buffer1[50];
    s saw;
    for (int i = 0; i < 2; i++) {
        getFunctionNameFromStream(buffer1,4);
        saw.m1[i] = stoi(buffer1);
        cout << saw.m1[i] << " ";
    }
    cout << endl;
    
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            getFunctionNameFromStream(buffer1,4);
            saw.m2[i][j] = stoi(buffer1);
            cout << saw.m2[i][j] << " ";
        }
    }
    cout << endl;

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 4; k++) {
                getFunctionNameFromStream(buffer1,4);
                saw.m3[i][j][k] = stoi(buffer1);
                cout << saw.m3[i][j][k] << " ";
            }
        }
    }
    cout << endl;
    if (!RPCSTUBSOCKET-> eof()) {
        if (strcmp(f_name, "sqrt") == 0){ 
            __sqrt(saw);
        }
        
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

void getFunctionNameFromStream(char *buffer, unsigned int bufSize) {
    unsigned int i;
    char *bufp;    // next char to read
    bool readnull;
    ssize_t readlen;             // amount of data read from socket
    
    //
    // Read a message from the stream
    // -1 in size below is to leave room for null
    //
    readnull = false;
    bufp = buffer;
    for (i=0; i< bufSize; i++) {
        readlen = RPCSTUBSOCKET-> read(bufp, 1);  // read a byte
        // check for eof or error f
        if (readlen == 0) break;

        // check for null and bump buffer pointer
        if (*bufp++ == '\0') {
            readnull = true;
            break;
        }
    }

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
}


