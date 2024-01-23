#ifndef __PLAYER_VALUE_H
#define __PLAYER_VALUE_H

namespace PLAYER {

    typedef union Value {
        float f;
        int i;
        byte b;

        Value() : f(NULL) {}
        Value(float v) : f(v) {}
        Value(int v) : i(v) {}
        Value(byte v) : b(v) {}
    } Value;
}
#endif