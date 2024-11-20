// Stub file generated for floatarithmetic

#include "floatarithmetic.idl"
#include "rpcstubhelper.h"

using namespace std;
using namespace C150NETWORK;

void getFunctionNameFromStream(char *buffer, unsigned bufSize) {
    char *bufp = buffer;     
    ssize_t readlen;  
    for (unsigned i = 0; i < bufSize; i++) {
        readlen = RPCSTUBSOCKET->read(bufp, 1);
        if (readlen == 0) break;
        if (*bufp++ == '\0') break;
    }
}

void __add(float x, float y) {
    float output = add(x, y);
    char *outputBuffer = reinterpret_cast<char*>(&output);
    RPCSTUBSOCKET->write(outputBuffer, sizeof(float));
}

void __divide(float x, float y) {
    float output = divide(x, y);
    char *outputBuffer = reinterpret_cast<char*>(&output);
    RPCSTUBSOCKET->write(outputBuffer, sizeof(float));
}

void __multiply(float x, float y) {
    float output = multiply(x, y);
    char *outputBuffer = reinterpret_cast<char*>(&output);
    RPCSTUBSOCKET->write(outputBuffer, sizeof(float));
}

void __subtract(float x, float y) {
    float output = subtract(x, y);
    char *outputBuffer = reinterpret_cast<char*>(&output);
    cout << *reinterpret_cast<float*>(outputBuffer);
    RPCSTUBSOCKET->write(outputBuffer, sizeof(float));
}

void dispatchFunction() {
    cout << "in dispatch" << endl;
    char buffer[50];
    getFunctionNameFromStream(buffer, sizeof(buffer));
    string f_name(buffer);
    cout << f_name << endl;

    if (!RPCSTUBSOCKET-> eof()) {
	if (f_name == "add") {
	    char buffer0[sizeof(float)];
    	    RPCSTUBSOCKET->read(buffer0, sizeof(float));
    	    float x = *reinterpret_cast<float*>(buffer0);

	    char buffer1[sizeof(float)];
    	    RPCSTUBSOCKET->read(buffer1, sizeof(float));
    	    float y = *reinterpret_cast<float*>(buffer1);

	    __add (x, y);
	}

	if (f_name == "divide") {
	    char buffer0[sizeof(float)];
    	    RPCSTUBSOCKET->read(buffer0, sizeof(float));
    	    float x = *reinterpret_cast<float*>(buffer0);

	    char buffer1[sizeof(float)];
    	    RPCSTUBSOCKET->read(buffer1, sizeof(float));
    	    float y = *reinterpret_cast<float*>(buffer1);

	    __divide (x, y);
	}

	if (f_name == "multiply") {
	    char buffer0[sizeof(float)];
    	    RPCSTUBSOCKET->read(buffer0, sizeof(float));
    	    float x = *reinterpret_cast<float*>(buffer0);

	    char buffer1[sizeof(float)];
    	    RPCSTUBSOCKET->read(buffer1, sizeof(float));
    	    float y = *reinterpret_cast<float*>(buffer1);

	    __multiply (x, y);
	}

	if (f_name == "subtract") {
	    char buffer0[sizeof(float)];
    	    RPCSTUBSOCKET->read(buffer0, sizeof(float));
    	    float x = *reinterpret_cast<float*>(buffer0);

	    char buffer1[sizeof(float)];
    	    RPCSTUBSOCKET->read(buffer1, sizeof(float));
    	    float y = *reinterpret_cast<float*>(buffer1);

	    __subtract (x, y);
	}

    }
}