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

    typedef struct OscCtrls {
        PotF8 amp;
        PotF fre;
        Pot note;
        Pot tune;
        PotF8 psw;
        Pot wave;
    } OscCtrls;

    #define OscCtrlCount (sizeof(OscCtrls) / sizeof(PotBase))

    class Osc : public Elem {
    private: Pot* note_;
    private: double timer;
    private: double smp_;
    public:
        Osc();
        OscCtrls* controls() { return (OscCtrls*)controls_; }

        void reset();
        void assignControls(PotBase* controls);
        void setFromStream(byte*& stream);
        float run(Arg params = (void*)NULL);
        void setNoteControl(Pot* note);
    };
}

#endif
