#ifndef __PLAYER_VALUE_H
#define __PLAYER_VALUE_H

#include "basedef.h"

namespace PLAYER {

    typedef union Value {
        float f;
        int i;
        byte b;

        Value() : i(0) {}
        Value(float v) : f(v) {}
        Value(int v) : i(v) {}
        Value(byte v) { i = 0; b = v; }

        bool operator==(Value v) {
            return i == v.i;
        }
    } Value;
}
#endif