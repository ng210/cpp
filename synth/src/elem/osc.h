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

    typedef struct OscCtrls_ : ElemCtrls {
        PotF fre;
        Pot note;
        Pot tune;
        PotF8 psw;
        Pot wave;
    } OscCtrls;

    #define OscCtrlCount (sizeof(OscCtrls) / sizeof(Pot*))

    class Osc : public Elem {
    private: PROP_R(OscCtrls*, controls);
    private: Pot* note;
    private: float smp;
    private: double timer;
    public:
        Osc();
        void reset();
        void assignControls(PotBase* controls);
        void setFromStream(byte* stream);
        float run(Arg params = (void*)NULL);
        void setNoteControl(Pot* note);
    };
}

#endif
