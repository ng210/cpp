#ifndef __PLAYER_INPUT_H
#define __PLAYER_INPUT_H

#include "basedef.h"
#include "base/handler.h"
#include "base/stream.h"
#include "player/src/value.h"

NS_FW_BASE_USE

namespace PLAYER {

    typedef enum InputType {
        InputTypeB,
        InputTypeF,
        InputTypeF8
    } InputType;

    class Input {
    public:
        Input();
        Value min;
        Value max;
        Value step;
        Value* value;
        InputType type;
        Handler<Value> set;
        int size;

        Input(Value*);
        virtual ~Input();

        virtual void initialize(Value inMin, Value inMax, Value inStep);
        virtual void readFromStream(byte* stream);
        virtual void check();

        virtual void setFromNormalized(float v);

        virtual void inc(int count);
        virtual void dec(int count);

        virtual float getNormalized();
        virtual void getValueAsString(char* str, int len);

        virtual void writeToStream(Stream* stream);
        virtual void writeValueToStream(Stream* stream);

        static int setter(void*, Value, void* = NULL);
    };

    // derived classes of Input MUST NOT add new member variables
    // only member methods and overrides are allowed!

    class InputF : public Input {
    protected:
        InputF();
    public:
        InputF(Value*);

        void readFromStream(byte* stream);

        void check();

        void setFromNormalized(float v);

        void inc(int count);
        void dec(int count);

        float getNormalized();
        void getValueAsString(char* str, int len);

        void writeToStream(Stream* stream);
        void writeValueToStream(Stream* stream);

        static int setter(void*, Value, void* = NULL);
    };

    class InputF8 : public Input {
        Value bValue_;
    public:
        InputF8(Value*);
        Value* pValue;
        static int setter(void*, Value, void* = NULL);
    };
}

#endif