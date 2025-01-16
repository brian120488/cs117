// Proxy file generated for floatarithmetic

#include "floatarithmetic.idl"
#include "rpcproxyhelper.h"

using namespace std;
using namespace C150NETWORK;

float add(float x, float y) {
    string name = "add";
    RPCPROXYSOCKET->write(name.c_str(), name.length() + 1);

    char* arg0 = reinterpret_cast<char*>(&x);
    RPCPROXYSOCKET->write(arg0, sizeof(float));

    char* arg1 = reinterpret_cast<char*>(&y);
    RPCPROXYSOCKET->write(arg1, sizeof(float));

    char readBuffer[sizeof(float)];
    RPCPROXYSOCKET->read(readBuffer, sizeof(float));
    return *reinterpret_cast<float*>(readBuffer);
}

float divide(float x, float y) {
    string name = "divide";
    RPCPROXYSOCKET->write(name.c_str(), name.length() + 1);

    char* arg0 = reinterpret_cast<char*>(&x);
    RPCPROXYSOCKET->write(arg0, sizeof(float));

    char* arg1 = reinterpret_cast<char*>(&y);
    RPCPROXYSOCKET->write(arg1, sizeof(float));

    char readBuffer[sizeof(float)];
    RPCPROXYSOCKET->read(readBuffer, sizeof(float));
    return *reinterpret_cast<float*>(readBuffer);
}

float multiply(float x, float y) {
    string name = "multiply";
    RPCPROXYSOCKET->write(name.c_str(), name.length() + 1);

    char* arg0 = reinterpret_cast<char*>(&x);
    RPCPROXYSOCKET->write(arg0, sizeof(float));

    char* arg1 = reinterpret_cast<char*>(&y);
    RPCPROXYSOCKET->write(arg1, sizeof(float));

    char readBuffer[sizeof(float)];
    RPCPROXYSOCKET->read(readBuffer, sizeof(float));
    return *reinterpret_cast<float*>(readBuffer);
}

float subtract(float x, float y) {
    string name = "subtract";
    RPCPROXYSOCKET->write(name.c_str(), name.length() + 1);

    char* arg0 = reinterpret_cast<char*>(&x);
    RPCPROXYSOCKET->write(arg0, sizeof(float));

    char* arg1 = reinterpret_cast<char*>(&y);
    RPCPROXYSOCKET->write(arg1, sizeof(float));

    char readBuffer[sizeof(float)];
    RPCPROXYSOCKET->read(readBuffer, sizeof(float));
    float ans = *reinterpret_cast<float*>(readBuffer);
    return ans;
}