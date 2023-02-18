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

    typedef enum PotType {
        PotTypeB,
        PotTypeF,
        PotTypeF8
    } PotType;

    class PotBase {
    public:
        S min;
        S max;
        S step;
        S value;
        PotBase();
        PotBase(S min, S max, S step, S value);
        virtual void init(S min, S max, S step, S value);
        virtual void setFromStream(byte*& stream);

        // editor functions
        virtual S inc(int count) = 0;
        virtual S dec(int count) = 0;
        virtual float getNormalized() = 0;
        virtual S setFromNormalized(float v) = 0;
        virtual void getValueAsString(char* str, int len) = 0;

        PotType type;
    };

    class Pot : public PotBase {
    public:
        Pot();
        void setFromStream(byte*& stream);
        S inc(int count);
        S dec(int count);
        float getNormalized();
        S setFromNormalized(float v);
        void getValueAsString(char* str, int len);
    };

    class PotF : public PotBase {
    public:
        PotF();
        void setFromStream(byte*& stream);
        S inc(int count);
        S dec(int count);
        float getNormalized();
        S setFromNormalized(float v);
        void getValueAsString(char* str, int len);
    };

    class PotF8 : public PotF {
    public:
        PotF8();
        void init(S min, S max, S step, S value);
        void setFromStream(byte*& stream);
        float getNormalized();
        S setFromNormalized(float v);
    };
}

#endif