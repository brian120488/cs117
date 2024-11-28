#include <string>
using namespace std;

#include "structs.idl"
#include "stdlib.h"

Person findPerson(ThreePeople tp) {
    return tp.p1;
}

int area(rectangle r) {
    return r.x * r.y;
}