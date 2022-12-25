#ifndef __VOICE_H
#define __VOICE_H


#include "../elem/elem.h"
#include "../elem/env.h"
#include "../elem/osc.h"
#include "../elem/lfo.h"
#include "../elem/flt.h"
#include "synth.h"

NS_FW_BASE_USE
namespace SYNTH {

    class Voice {
    private: PROP_R(Pot, velocity);
    private: PROP_R(Pot, note);
    public:
        Env* envelopes;
        Osc* oscillators;
        Lfo* lfos;
        Flt* filters;

        Voice();
        virtual ~Voice();

        void freeResources();
        void setNote(byte node, byte velocity);
        void setSamplingRate(float* samplingRate);
        float run();
        inline bool isActive() { return envelopes[0].isActive(); }
        inline int getTicks() { return envelopes[0].ticks(); }
    };
}

#endif