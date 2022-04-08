#ifndef __FLT_H
#define __FLT_H

#include "mdl.h"

NS_FW_BASE_USE
namespace SYNTH {

    typedef enum FltMode_ {
        FmNoPass = 0,
        FmLowPass = 1,
        FmBandPass = 2,
        FmHighPass = 4,

        FmAllPass = 7
    } FltMode;


    typedef struct FltCtrls_ : MdlCtrls {
        PotF8 cut;
        PotF8 res;
        PotF8 mod;
        Pot mode;
    } FltCtrls;

    typedef struct FltCoeffs_ {
        float ai[3];
        float bi[3];
        float ci[3];
        float ui[2];
        float vi[2];
        float lp[2];
        float hp[2];
    } FltCoeffs;

    #define FltCtrlCount (sizeof(FltCtrls)/sizeof(Pot*))

    class Flt : public Mdl {
    private: PROP_R(FltCtrls*, controls);
    private: FltCoeffs coeffs;
    private: PROP_R(float, theta);
    public:
        Flt();
        void samplingRate(float* smpRate);
        void update(float cut);
        void assignControls(Pot* controls);
        void setFromStream(byte* stream);
        float run(void* params);
    };
}

#endif
