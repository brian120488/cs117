// Stub file generated for arithmetic

#include "arithmetic.idl"
#include "rpcstubhelper.h"

using namespace std;
using namespace C150NETWORK;

void getFunctionNameFromStream(char *buffer, unsigned bufSize) {
    char *bufp = buffer;     
    ssize_t readlen;
    for (unsigned i = 0; i < bufSize; i++) {
        readlen = RPCSTUBSOCKET->read(bufp, 1);
		cout << i << " " << bufp << endl;
        if (readlen == 0) break;
        if (*bufp++ == '\0') break;
    }
}

void __add(int x, int y) {
    int output = add(x, y);
    char *outputBuffer = reinterpret_cast<char*>(&output);
    RPCSTUBSOCKET->write(outputBuffer, sizeof(int));
}

void __divide(int x, int y) {
    int output = divide(x, y);
    char *outputBuffer = reinterpret_cast<char*>(&output);
    RPCSTUBSOCKET->write(outputBuffer, sizeof(int));
}

void __multiply(int x, int y) {
    int output = multiply(x, y);
    char *outputBuffer = reinterpret_cast<char*>(&output);
    RPCSTUBSOCKET->write(outputBuffer, sizeof(int));
}

void __subtract(int x, int y) {
    int output = subtract(x, y);
    char *outputBuffer = reinterpret_cast<char*>(&output);
    RPCSTUBSOCKET->write(outputBuffer, sizeof(int));
}

void dispatchFunction() {
    char buffer[50];
    getFunctionNameFromStream(buffer, sizeof(buffer));
    string f_name(buffer);

    if (!RPCSTUBSOCKET-> eof()) {
        if (f_name == "add") {
            char buffer0[sizeof(int)];
            RPCSTUBSOCKET->read(buffer0, sizeof(int));
            int x = *reinterpret_cast<int*>(buffer0);

            char buffer1[sizeof(int)];
            RPCSTUBSOCKET->read(buffer1, sizeof(int));
            int y = *reinterpret_cast<int*>(buffer1);

            __add (x, y);
        }

	if (f_name == "divide") {
	    char buffer0[sizeof(int)];
    	    RPCSTUBSOCKET->read(buffer0, sizeof(int));
    	    int x = *reinterpret_cast<int*>(buffer0);

	    char buffer1[sizeof(int)];
    	    RPCSTUBSOCKET->read(buffer1, sizeof(int));
    	    int y = *reinterpret_cast<int*>(buffer1);

	    __divide (x, y);
	}

	if (f_name == "multiply") {
	    char buffer0[sizeof(int)];
    	    RPCSTUBSOCKET->read(buffer0, sizeof(int));
    	    int x = *reinterpret_cast<int*>(buffer0);

	    char buffer1[sizeof(int)];
    	    RPCSTUBSOCKET->read(buffer1, sizeof(int));
    	    int y = *reinterpret_cast<int*>(buffer1);

	    __multiply (x, y);
	}

	if (f_name == "subtract") {
	    char buffer0[sizeof(int)];
    	    RPCSTUBSOCKET->read(buffer0, sizeof(int));
    	    int x = *reinterpret_cast<int*>(buffer0);

	    char buffer1[sizeof(int)];
    	    RPCSTUBSOCKET->read(buffer1, sizeof(int));
    	    int y = *reinterpret_cast<int*>(buffer1);

	    __subtract (x, y);
	}

    }
}