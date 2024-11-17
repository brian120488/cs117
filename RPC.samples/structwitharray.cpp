// --------------------------------------------------------------
//
//                        structswitharray.cpp
//
//        Author:         
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

#include "structwitharray.idl"
#include "stdlib.h"
#include <iostream>


int sqrt(s swa) {
    int sum = 0;
    for (int i = 0; i < 2; i++) {
        sum += swa.m1[i];
    }

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            sum += swa.m2[i][j];
        }
    }

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 4; k++) {
                 sum += swa.m3[i][j][k];
            }
           
        }
    }
    return sum;
}






