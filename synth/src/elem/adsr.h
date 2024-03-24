#ifndef __ADSR_H
#define __ADSR_H

// Attack Decay Sustain Release envelope

#include "player/src/value.h"
#include "synth/src/elem/env.h"

NS_FW_BASE_USE
using namespace PLAYER;
namespace SYNTH {

    typedef struct AdsrValues_ {
        Value amp, atk, dec, sus, rel;
    } AdsrValues;


    class Adsr : public Env {
    protected:
        AdsrValues* values();
    public:
        Adsr();
        void setGate(byte velocity);
        float run(Arg params = (void*)NULL);
    };
}
#endif