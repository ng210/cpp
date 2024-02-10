
#ifndef __LFO_H
#define __LFO_H

#include "elem.h"

NS_FW_BASE_USE
namespace SYNTH {

    typedef struct LfoValues_ {
        Value amp, fre;
    } LfoValues;

    class Lfo : public Elem {
    private: float smp;
    private: double timer;
    protected: PROP(LfoValues*, values);
    public:
        Lfo();

        void reset();
        float run(Arg params = (void*)NULL);

        static float* samplingRate;
    };
}

#endif