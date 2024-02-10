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

    typedef struct FltValues_ {
        Value cut, res, mode;
    } FltValues;

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

    class Flt : public Elem {
    protected: FltStage* stages_[5];   // 5x2 = max 10 poles
    protected: PROP_R(int, stageCount);
    protected: PROP_R(int, poleCount);
        static float* samplingRate_;
    protected: PROP(FltValues*, values);
    public:
        Flt(int poleCount = 2);
        ~Flt();

        void update(float cut);
        void createStages(int poleCount);
        float run(Arg params = (void*)NULL);

        FltStage* stage(int si) { return stages_[si]; }

        static double cutoffTable[256];
        static double* linearFactors[];
        static void initialize();
    };
}

#endif
