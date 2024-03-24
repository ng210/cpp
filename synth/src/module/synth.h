#ifndef __SYNTH_H
#define __SYNTH_H

#include "collection/array.h"
#include "synth/src/module/synth-base.h"
#include "synth/src/module/synth-def.h"

NS_FW_BASE_USE
namespace SYNTH {

	typedef struct SynthValues_ {
		AdsrValues amAdsr, fmAdsr, pmAdsr, ftAdsr;
		LfoValues lfo1, lfo2;
		OscValues osc1, osc2;
		FltValues flt1;
	} SynthValues;

    class Synth : public SynthBase {
		static SetupVoiceHandler setupVoiceHandler_;
		static VoiceRenderer renderVoiceHandler_;
		//VoiceHandler freeVoiceHandler;
		//VoiceSetNoteHandler setNoteVoiceHandler;
	protected: SynthValues values_;
    public:
        Synth(int voiceCount = 3);
        ~Synth();

		Value* getValues();
    };
}

#endif
