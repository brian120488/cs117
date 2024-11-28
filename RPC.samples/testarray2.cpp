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

#include "testarray2.idl"
#include "stdlib.h"
#include <iostream>


int sqrt(int x[3], int y[3][2], int z[3][2][2]) {
    int sum = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            sum += y[i][j];
        }
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                sum += z[i][j][k];
            }
        }
    }

    for (int i = 0; i < 3; i++) {
        sum += x[i];
    }

    return sum;
}






