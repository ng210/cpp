#ifndef __ADSR_H
#define __ADSR_H

// Attack Decay Sustain Release envelope

#include "synth/src/elem/env.h"

NS_FW_BASE_USE
namespace SYNTH {

    typedef struct AdsrCtrls {
        PotF amp;
        PotF dc;
        Pot atk;
        Pot dec;
        PotF8 sus;
        Pot rel;
    } AdsrCtrls;

    #define AdsrCtrlCount (sizeof(AdsrCtrls) / sizeof(PotBase))

    class Adsr : public Env {
    private: PROP_R(AdsrCtrls*, controls);
    protected: byte gate_;
    public:
        Adsr();

        void assignControls(PotBase* controls);
        void setGate(byte velocity);
        void setFromStream(byte*& stream);
        float run(Arg params = (void*)NULL);
    };
}
#endif