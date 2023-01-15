
#ifndef __LFO_H
#define __LFO_H

#include "elem.h"

NS_FW_BASE_USE
namespace SYNTH {

    typedef struct LfoCtrls {
        PotF amp;
        PotF fre;
    } LfoCtrls;

    #define LfoCtrlCount (sizeof(LfoCtrls)/sizeof(Pot*))

    class Lfo : public Elem {
    private: PROP_R(LfoCtrls*, controls);
    private: float smp;
    private: double timer;
    public:
        Lfo();
        void reset();
        void assignControls(PotBase* controls);
        void setFromStream(byte* stream);
        float run(Arg params = (void*)NULL);

        static float* samplingRate;
        static void initialize(float* samplingRate);
    };

}

#endif