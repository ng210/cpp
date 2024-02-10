#ifndef __OSC_H
#define __OSC_H

#include "elem.h"

NS_FW_BASE_USE
namespace SYNTH {

    typedef enum OscWaveform_ {
        WfNone = 0,
        WfSinus = 1,
        WfTriangle = 2,
        WfSaw = 4,
        WfPulse = 8,
        WfNoise = 16
    } OscWaveform;

    typedef struct OscValues_ {
        Value amp;
        Value fre;
        Value note;
        Value tune;
        Value psw;
        Value wave;
    } OscValues;

    class Osc : public Elem {
    private: Value* note_;
    private: double timer;
    public: double smp;
    protected: PROP(OscValues*, values);
    public:
        Osc();

        void reset();
        float run(Arg params = (void*)NULL);
        void setNoteControl(Value* note);
    };
}

#endif
