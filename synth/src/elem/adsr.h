#ifndef __ADSR_H
#define __ADSR_H

// Attack Decay Sustain Release envelope

#include "synth/src/elem/env.h"

NS_FW_BASE_USE
namespace SYNTH {

    typedef struct AdsrCtrls {
        PotF amp;
        Pot atk;
        Pot dec;
        PotF8 sus;
        Pot rel;
    } AdsrCtrls;

    #define AdsrCtrlCount (sizeof(AdsrCtrls) / sizeof(PotBase))

    class Adsr : public Env {
    protected: byte gate_;
    public:
        Adsr();
        AdsrCtrls* controls() { return (AdsrCtrls*)controls_; }
        void setGate(byte velocity);
        float run(Arg params = (void*)NULL);
    };
}
#endif