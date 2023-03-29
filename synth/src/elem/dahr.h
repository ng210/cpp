#ifndef __DAHR_H
#define __DAHR_H

// Delay Attack Hold Release

#include "synth/src/elem/env.h"

NS_FW_BASE_USE
namespace SYNTH {

    typedef struct DahrCtrls {
        PotF amp;
        PotF dc;
        Pot del;
        Pot atk;
        Pot hld;
        Pot rel;
    } DahrCtrls;

    #define DahrCtrlCount (sizeof(DahrCtrls) / sizeof(PotBase))

    class Dahr : public Env {
    protected: PROP_R(double, smp);
    protected: PROP_R(double, rate2);
    public:
        Dahr();

        DahrCtrls* controls() { return (DahrCtrls*)controls_; }

        void assignControls(PotBase* controls);
        void setFromStream(byte*& stream);
        float run(Arg params = (void*)NULL);

        void setGate(byte velocity);
    };
}
#endif