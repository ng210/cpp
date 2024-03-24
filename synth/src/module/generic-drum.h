#ifndef __GENERIC_DRUM_H
#define __GENERIC_DRUM_H

#include "base/handler.h"
#include "collection/array.h"
#include "synth/src/module/synth-base.h"
#include "synth/src/elem/dahr.h"
#include "synth/src/elem/adsr.h"
#include "synth/src/elem/lfo.h"
#include "synth/src/elem/osc.h"
#include "synth/src/elem/flt.h"

NS_FW_BASE_USE
namespace SYNTH {

    typedef struct GenericDrumValues_ {
        Value type;
        DahrValues dahr[4];
        FltValues flt[2];
        OscValues osc[6];
        //Value amp[6];
        //Value fre[6];
    } GenericDrumValues;

    //typedef struct GenericDrumCtrlsPots {
    //    PotF gdFre[6];
    //    PotF8 gdAmp[6];
    //} GenericDrumCtrlsPots;

    //typedef struct GenericDrumCtrls {
    //    Value type;
    //    DahrCtrls dahr[4];
    //    FltCtrls flt[2];
    //    GenericDrumCtrlsPots pots;
    //} GenericDrumCtrls;

    //typedef enum GenericDrumCtrlIds {
    //    gdType,
    //    gdDahr1Amp, gdDahr1Del, gdDahr1Atk, gdDahr1Hld, gdDahr1Rel,
    //    gdDahr2Amp, gdDahr2Del, gdDahr2Atk, gdDahr2Hld, gdDahr2Rel,
    //    gdDahr3Amp, gdDahr3Del, gdDahr3Atk, gdDahr3Hld, gdDahr3Rel,
    //    gdDahr4Amp, gdDahr4Del, gdDahr4Atk, gdDahr4Hld, gdDahr4Rel,
    //    gdFlt1Cut, gdFlt1Res, gdFlt1Mode,
    //    gdFlt2Cut, gdFlt2Res, gdFlt2Mode,
    //    gdFreq1, gdFreq2, gdFreq3, gdFreq4, gdFreq5, gdFreq6,
    //    gdAmp1, gdAmp2, gdAmp3, gdAmp4, gdAmp5, gdAmp6
    //} GenericDrumCtrlIds;
    //#define GenericDrumCtrlCount sizeof(GenericDrumCtrls) / sizeof(PotBase)

    typedef enum DrumTypes {
        BassDrumType = 0,
        SnareDrumType,
        HihatType,
        ClapType,
        DefaultDrumType
    } DrumTypes;

    class GenericDrum;
    typedef void (RenderDrum)(float* buffer, Voice* v, int start, int end);
    typedef RenderDrum* LpRenderDrum;

    class GenericDrum : public SynthBase {
        static SetupVoiceHandler setupVoiceHandler_;
        static VoiceRenderer renderVoiceHandler_;

        static RenderDrum renderDefault;
        static RenderDrum renderHihat;
        //static RenderDrum renderClap;
    protected: float runFilter(Flt* flt, float cut, float input);
    protected: GenericDrumValues* values_;
    public: PROP_R(LpRenderDrum, render);
    public:
        GenericDrum();
        ~GenericDrum();

        void values(Value* values);
        //Handler<DrumTypes> setType;

        Value* getValues();

        static int typeSetter(void*, Value, void* = NULL);
    };
}
#endif
