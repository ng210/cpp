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
    public:
        double ai_[3];    // nominator coeffs
        double bi_[3];    // LP denominator coeffs
        double ci_[3];    // HP denominator coeffs
        double ui_[3];    // LP inputs
        double vi_[3];    // HP inputs
        double lp_[2];    // LP outputs
        double hp_[2];    // HP outputs
    public:
        FltStage();
        virtual void run(Arg) = 0;
        virtual void update(double, double) = 0;
    };

    class FltStage1Pole : public FltStage {
    public:
        void run(Arg);
        void update(double, double);
    };

    class FltStage2Pole : public FltStage {
        double linearFactor_;
    public:
        FltStage2Pole(double f);
        void run(Arg);
        void update(double, double);
    };

    #define FltCtrlCount (sizeof(FltCtrls)/sizeof(PotBase))

    class Flt : public Elem {
    protected: FltStage* stages_[5];   // 5x2 = max 10 poles
    //private: PROP_R(float, theta);
    protected: PROP_R(int, stageCount);
    protected: PROP_R(int, poleCount);
        static float* samplingRate_;
    public:
        Flt(int poleCount = 2);
        ~Flt();

        FltCtrls* controls() { return (FltCtrls*)controls_; }

        void update(float cut);
        void assignControls(PotBase* controls);
        void createStages(int poleCount);
        void setFromStream(byte*& stream);
        float run(Arg params = (void*)NULL);

        FltStage* stage(int si) { return stages_[si]; }

        static double cutoffTable[256];
        static double* linearFactors[];
        static void initialize();
    };
}

#endif
