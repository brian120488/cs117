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
#include "c150debug.h"
#include <stack>

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
  


void __sqrt(int x[3], int y[3][2], int z[3][2]) {

  //
  // Time to actually call the function 
  //
  c150debug->printf(C150RPCDEBUG,"testarray1.stub.cpp: invoking sqrt()");
  cout << "Sqrting...\n";
  int output = sqrt(x, y, z);
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

    int *arr = new int[3];
    stringstream ss(s);
    string num;
    int index = 0;
    while (getline(ss, num, ' ')) {
        if (index >= 3) break;  // Ensure no overflow in the array
        arr[index++] = stoi(num);
    }
    return arr;
}


int (*string_to_array2d(string s))[2] {
    // remove []
    cout << "STRING: " << s << endl;
    s.erase(0, 1);
    s.pop_back();

    int (*arr)[2] = new int[3][2];

    stringstream ss(s);
    string num;
    int row = 0, col = 0;

    while (getline(ss, num, ' ')) {
        // Remove any remaining brackets or spaces
        num.erase(std::remove(num.begin(), num.end(), '['), num.end());
        num.erase(std::remove(num.begin(), num.end(), ']'), num.end());
        num.erase(std::remove(num.begin(), num.end(), ' '), num.end());
        if (num == "") continue;
        cout << "NUM: " << num << endl;
        // Convert string to integer and assign to 2D array
        arr[row][col] = std::stoi(num);

        // Move to next column or row
        col++;
        if (col == 2) {  // Reset column and move to next row
            col = 0;
            row++;
        }
        if (row == 3) break;  // Stop if we have filled the 3x2 array
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
    char functionNameBuffer[50];

    int (*arr)[2] = string_to_array2d("[[0 0 ] [0 0 ] [1 1 ] ]");
    for (int r = 0; r < 3; r++) {
        cout << "[";
        for (int c = 0; c < 2; c++) {
            cout << arr[r][c] << " ";
        }
        cout << "]";
    }

    //
    // Read the function name from the stream -- note
    // REPLACE THIS WITH YOUR OWN LOGIC DEPENDING ON THE 
    // WIRE FORMAT YOU USE
    //
    Function *fp = getFunctionFromStream(functionNameBuffer,sizeof(functionNameBuffer));
    if (fp == NULL) return;
    Function f = *fp;


    // TODO: implement function that turns string array to array
    int *x = string_to_array(f.args[0]);
    int (*y)[2] = string_to_array2d(f.args[1]);
    int (*z)[2] = string_to_array2d(f.args[2]);
    cout << f.name << "(" << f.args[0] << ", " << f.args[1] << ", " << f.args[2] << endl;
    cout <<"hi\n";
    if (!RPCSTUBSOCKET-> eof()) {
        cout <<"hi2\n";
        if (f.name == "sqrt"){ 
            cout <<"hi3\n";
            __sqrt(x, y, z);
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
            f->name.pop_back();
            cout << "NAMEEE: " << f->name;
            break;
        }
        // check for null and bump buffer pointer
        if (*bufp++ == '\0') {
            readnull = true;
            break;
        }
    }

    stack<string> stack;
    string curr_str = "";
    buffer = bufp;
    while (true) {
        int readlen =  RPCSTUBSOCKET->read(bufp, 1); // Reading 1 character at a time
        if (readlen == 0) break; // Exit if there's nothing more to read
        if (*bufp == ']') { // End of array
            if (!curr_str.empty()) {
                stack.push(curr_str);
                curr_str = "";
            }

            string array_content = ""; 
            while (!stack.empty() && stack.top() != "[") {
                array_content = stack.top() + array_content;
                stack.pop();
            }
            stack.pop(); // Remove the '['
            stack.push("[" + array_content + "]");

        } else if (*bufp == '}') { // End of struct
            // if (!curr_str.empty()) {
            //     stack.push(curr_str);
            //     curr_str = "";
            // }

            // std::string struct_content = "";
            // while (!stack.empty() && stack.top() != "{") {
            //     struct_content = stack.top() + struct_content;
            //     stack.pop();
            // }
            // if (!stack.empty()) stack.pop(); // Remove the '{'
            // stack.push("{" + struct_content + "}");

        } else if (*bufp == ')') { // End of function
            if (!curr_str.empty()) {
                stack.push(curr_str);
                curr_str = "";
            }
        } else if (*bufp == '[') { // Element separator
            stack.push("[");
        } 
        else if (*bufp == ' ') { // Element separator
            if (!curr_str.empty()) {
                stack.push(curr_str + " ");
                curr_str = "";
            }
        } 
        else if (*bufp == ',') { // Argument separator
            if (!curr_str.empty()) {
                stack.push(curr_str);
                curr_str = "";
            }
        } else if (isalnum(*bufp)) { // Alphanumeric character
            curr_str += *bufp;
        }
        if (*bufp++ == '\0') {
            readnull = true;
            break;
        }

        //  cout << "STACK: " << stack.top() << endl;
    }

    while (!stack.empty()) {
        f->args.insert(f->args.begin(), stack.top());
        stack.pop();
    }

    cout << "NAME: " << f->name << endl;
    for (int i = 0; i <(int) f->args.size(); i++){
        cout << "ARG " << i << ": " << f->args[i] << endl;
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
    cout << "Returning func\n";
    return f;
}


