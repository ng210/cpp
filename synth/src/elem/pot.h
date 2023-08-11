#ifndef __POT_H
#define __POT_H

#include "basedef.h"
#include "synth/src/handler.h"

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

    //CREATEHANDLER1(Setter, S);

    class PotBase {
    public:
        S min;
        S max;
        S step;
        S value;
        PotType type;

        PotBase();
        PotBase(S min, S max, S step, S value);

        Handler<S> set;
        //SetterHandler set;

        virtual void init(S min, S max, S step, S value);

        virtual void inc(int count) = 0;
        virtual void dec(int count) = 0;
        virtual int size() = 0;

        virtual float getNormalized() = 0;
        virtual void getValueAsString(char* str, int len) = 0;
        virtual void setFromStream(byte*& stream) = 0;
        virtual void setFromNormalized(float v) = 0;
        virtual void writeToStream(byte*& stream) = 0;

        static int setter(void*, S);
        static int setterF8(void*, S);
    };

    // derived classes of PotBase MUST NOT add new member variables
    // only member methods and overrides are allowed!

    class Pot : public PotBase {
    public:
        Pot();
        
        void inc(int count);
        void dec(int count);
        int size();

        float getNormalized();
        void getValueAsString(char* str, int len);
        void setFromNormalized(float v);
        void setFromStream(byte*& stream);
        void writeToStream(byte*& stream);
    };

    class PotF : public PotBase {
    public:
        PotF();
        void inc(int count);
        void dec(int count);
        int size();

        float getNormalized();
        void getValueAsString(char* str, int len);
        void setFromNormalized(float v);
        void setFromStream(byte*& stream);        
        void writeToStream(byte*& stream);
    };

    class PotF8 : public PotF {
    public:
        PotF8();
        void init(S min, S max, S step, S value);
        int size();

        void getValueAsString(char* str, int len);
        void setFromStream(byte*& stream);
        void writeToStream(byte*& stream);
    };
}

#endif