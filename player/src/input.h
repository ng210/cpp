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

    class InputBase {
    protected:
        PROP_R(Value*, pValue);
        PROP_R(Value*, value);
    protected:
        Value bValue_;
    public:
        Value min;
        Value max;
        Value step;
        InputType type;
        int size;
        Handler<Value> set;

        InputBase();
        InputBase(Value*);
        virtual ~InputBase();

        virtual void setValue(Value* v);
        virtual void setup(Value inMin, Value inMax, Value inStep);
        virtual void readValueFromStream(byte*& stream) = 0;
        virtual void readFromStream(byte*& stream) = 0;
        virtual void check() = 0;

        virtual void setFromNormalized(float v) = 0;

        virtual void inc(int count) = 0;
        virtual void dec(int count) = 0;
        virtual float getRange() = 0;

        virtual float getNormalized() = 0;
        virtual void getValueAsString(char* str, int len) = 0;

        virtual void writeToStream(Stream* stream) = 0;
        virtual void writeValueToStream(Stream* stream) = 0;
    };

    class Input : public InputBase {
        void initialize();
    public:
        Input();

        Input(Value*);
        ~Input();

        void readValueFromStream(byte*& stream);
        void readFromStream(byte*& stream);
        void check();

        void setFromNormalized(float v);

        void inc(int count);
        void dec(int count);
        float getRange();

        float getNormalized();
        void getValueAsString(char* str, int len);

        void writeToStream(Stream* stream);
        void writeValueToStream(Stream* stream);

        static int setter(void*, Value, void* = NULL);
    };

    // derived classes of Input MUST NOT add new member variables
    // only member methods and overrides are allowed!

    class InputF : public InputBase {
        void initialize();
    public:
        InputF();
        InputF(Value*);

        void readValueFromStream(byte*& stream);
        void readFromStream(byte*& stream);

        void check();

        void setFromNormalized(float v);

        void inc(int count);
        void dec(int count);
        float getRange();

        float getNormalized();
        void getValueAsString(char* str, int len);

        void writeToStream(Stream* stream);
        void writeValueToStream(Stream* stream);

        static int setter(void*, Value, void* = NULL);
    };

    class InputF8 : public Input {
        void initialize();
    public:
        InputF8();
        InputF8(Value*);

        void setValue(Value* v);
        static int setter(void*, Value, void* = NULL);
    };
}

#endif