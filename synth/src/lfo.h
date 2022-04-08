
#ifndef __LFO_H
#define __LFO_H

#include "mdl.h"

NS_FW_BASE_USE
namespace SYNTH {

    typedef struct LfoCtrls_ : MdlCtrls {
        //PotF dc;
        PotF fre;
    } LfoCtrls;

    #define LfoCtrlCount (sizeof(LfoCtrls)/sizeof(Pot*))

    class Lfo : public Mdl {
    private: PROP_R(LfoCtrls*, controls);
    private: float smp;
    private: double timer;
    public:
        Lfo();
        void reset();
        void assignControls(Pot* controls);
        void setFromStream(byte* stream);
        float run(void* params = NULL);
    };

}

#endif