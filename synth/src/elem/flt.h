#ifndef __FLT_H
#define __FLT_H

#include "elem.h"

NS_FW_BASE_USE
namespace SYNTH {

    typedef enum FltMode_ {
        FmNoPass = 0,
        FmLowPass = 1,
        FmBandPass = 2,
        FmHighPass = 4,

        FmAllPass = 7
    } FltMode;


    typedef struct FltCtrls {
        Pot cut;
        PotF8 res;
        PotF8 mod;
        Pot mode;
    } FltCtrls;

    class Flt;
    class FltStage {
        friend class Flt;
    protected:
        float ai_[3];    // nominator coeffs
        float bi_[3];    // LP denominator coeffs
        float ci_[3];    // HP denominator coeffs
        float ui_[3];    // LP inputs
        float vi_[3];    // HP inputs
        float lp_[2];    // LP outputs
        float hp_[2];    // HP outputs
    public:
        FltStage();
        virtual void run(Arg) = 0;
        virtual void update(float, float) = 0;
    };

    class FltStage1Pole : public FltStage {
    public:
        void run(Arg);
        void update(float, float);
    };

    class FltStage2Pole : public FltStage {
        float linearFactor_;
    public:
        FltStage2Pole(float f);
        void run(Arg);
        void update(float, float);
    };

    #define FltCtrlCount (sizeof(FltCtrls)/sizeof(Pot*))

    class Flt : public Elem {
    protected: PROP_R(FltCtrls*, controls);
    protected: FltStage* stages_[5];   // 5x2 = max 10 poles
    //private: PROP_R(float, theta);
    protected: PROP_R(int, stageCount);
    public:
        Flt(int poleCount = 2);
        ~Flt();

        void update(float cut);
        void assignControls(PotBase* controls);
        void createStages(int poleCount);
        void setFromStream(byte* stream);
        float run(Arg params = (void*)NULL);

        static float cutoffTable[256];
        static float* linearFactors[];
        static void initialize(float smpRate);
    };
}

#endif
