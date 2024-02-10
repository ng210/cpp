#ifndef __BASS_H
#define __BASS_H
#include "collection/array.h"
#include "synth/src/module/synth-base.h"
#include "synth/src/module/synth-def.h"

NS_FW_BASE_USE
namespace SYNTH {

    typedef struct BassValues_ {
        AdsrValues amAdsr, pmAdsr, ftAdsr;
        OscValues osc1, osc2;
        FltValues flt1;
    } BassValues;

#define BassCtrlCount (sizeof(BassCtrls)/sizeof(PotBase))

    class Bass : public SynthBase {
        VoiceHandler setupVoiceHandler;
        VoiceRenderer renderVoiceHandler;
        VoiceHandler freeVoiceHandler;
        VoiceSetNoteHandler setNoteVoiceHandler;

    protected: BassValues values_;
    public:
        Bass(int voiceCount = 1);
        virtual ~Bass();

        Value* getValues();
    };
}

#endif
