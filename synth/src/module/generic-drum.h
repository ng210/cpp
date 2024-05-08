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
    } GenericDrumValues;

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
        static RenderDrum renderClap;
    protected: float runFilter(Flt* flt, float cut, float input);
    protected: GenericDrumValues values_;
    public: PROP_R(LpRenderDrum, render);
    public:
        GenericDrum();
        ~GenericDrum();

        Value* getValues();

        static int typeSetter(void*, Value, void* = NULL);
    };
}
#endif
