#include <string>
using namespace std;

#include "structs.idl"
#include "stdlib.h"

string getFirstName(Person p) {
    return p.firstname;
}

Person findPerson(ThreePeople tp) {
    return tp.p1;
}

StructWithArrays test(StructWithArrays swa[2]) {
    return swa[1];
}
 
 Pain pain_func(Pain p) {
    return p;
 }

int area(rectangle r) {
    return r.x * r.y;
}