// --------------------------------------------------------------
//
//                        testarray1.cpp
//
//        Author: Manuel Pena         
//   
//
//        Trivial implementations of the routines declared
//        in arithmetic.idl. These are for testing: they
//        just print messages.
//
//     
// --------------------------------------------------------------

// IMPORTANT! WE INCLUDE THE IDL FILE AS IT DEFINES THE INTERFACES
// TO THE FUNCTIONS WE'RE IMPLEMENTING. THIS MAKES SURE THE
// CODE HERE ACTUALLY MATCHES THE REMOTED INTERFACE

#include "testarray1.idl"
#include "stdlib.h"
#include <iostream>


int sqrt(int x[24], int y[24]) {
    int match = 0;
    for (int i = 0; i < 24; i++) {
        std::cout << x[i] << " " << y[i] << std::endl;
        if (x[i] == y[i]) {
            match++;
        }
    }
    return match;
}


