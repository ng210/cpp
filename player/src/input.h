#ifndef __PLAYER_INPUT_H
#define __PLAYER_INPUT_H

#include "basedef.h"
#include "base/handler.h"

NS_FW_BASE_USE

namespace PLAYER {

    typedef union Value {
        float f;
        int i;
        byte b;

        Value() : f(0) {}
        Value(float v) : f(v) {}
        Value(int v) : i(v) {}
        Value(byte v) : i(v) {}
    } Value;

    typedef enum InputType {
        InputTypeB,
        InputTypeF,
        InputTypeF8
    } InputType;

    class InputBase {
    public:
        Value min;
        Value max;
        Value step;
        Value value;
        InputType type;

        InputBase();
        InputBase(Value min, Value max, Value step, Value value);

        Handler<Value> set;

        virtual void init(Value min, Value max, Value step, Value value);

        virtual void inc(int count) = 0;
        virtual void dec(int count) = 0;
        virtual int size() = 0;

        virtual float getNormalized() = 0;
        virtual void getValueAsString(char* str, int len) = 0;
        virtual void setFromStream(byte*& stream) = 0;
        virtual void setFromNormalized(float v) = 0;
        virtual void writeToStream(byte*& stream) = 0;

        static int setter(void*, Value, void* = NULL);
        //static int setterF8(void*, Value, void*);
    };

    // derived classes of InputBase MUValueT NOT add new member variables
    // only member methods and overrides are allowed!

    class Input : public InputBase {
    public:
        Input();

        void inc(int count);
        void dec(int count);
        int size();

        float getNormalized();
        void getValueAsString(char* str, int len);
        void setFromNormalized(float v);
        void setFromStream(byte*& stream);
        void writeToStream(byte*& stream);
    };

    class InputF : public InputBase {
    public:
        InputF();
        void inc(int count);
        void dec(int count);
        int size();

        float getNormalized();
        void getValueAsString(char* str, int len);
        void setFromNormalized(float v);
        void setFromStream(byte*& stream);
        void writeToStream(byte*& stream);
    };

    class InputF8 : public InputF {
    public:
        InputF8();
        void init(Value min, Value max, Value step, Value value);
        int size();

        void getValueAsString(char* str, int len);
        void setFromStream(byte*& stream);
        void writeToStream(byte*& stream);
    };
}

#endif