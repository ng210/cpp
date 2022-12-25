#ifndef __POT_H
#define __POT_H

#include "basedef.h"

NS_FW_BASE_USE
namespace SYNTH {

    #define SAMPLE_BUFFER_SIZE 256

    typedef union S {
        float f;
        byte b;

        S() : f(0) {}
        S(float v) : f(v) {}
        S(int v) : b(v) {}
    } S;


    class PotBase {
    public:
        S min;
        S max;
        S step;
        S value;
        PotBase();
        PotBase(S min, S max, S step, S value);
        virtual void init(S min, S max, S step, S value);
        virtual void setFromStream(byte*& stream) = 0;
    };

    class Pot : public PotBase {
    public:
        void setFromStream(byte*& stream);
    };

    class PotF : public PotBase {
    public:
        void setFromStream(byte*& stream);
    };

    class PotF8 : public PotBase {
    public:
        void init(S min, S max, S step, S value);
        void setFromStream(byte*& stream);
    };
}

#endif