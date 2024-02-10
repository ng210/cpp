#ifndef __DAHR_H
#define __DAHR_H

// Delay Attack Hold Release

#include "synth/src/elem/env.h"

NS_FW_BASE_USE
namespace SYNTH {

    typedef struct DahrValues_ {
        Value amp, del, atk, hld, rel;
    } DahrValues;


    class Dahr : public Env {
    protected: PROP_R(double, rate2);
    protected: PROP(DahrValues*, values);
    public:
        Dahr();

        float run(Arg params = (void*)NULL);

        void setGate(byte velocity);
    };
}
#endif