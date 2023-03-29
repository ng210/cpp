#ifndef __SYNTH_H
#define __SYNTH_H

#include "collection/array.h"
#include "synth/src/module/synth-base.h"
#include "synth/src/module/synth-def.h"

NS_FW_BASE_USE
namespace SYNTH {

    typedef struct SynthCtrls_ {
        AdsrCtrls amAdsr, fmAdsr, pmAdsr, ftAdsr;
        LfoCtrls lfo1, lfo2;
        OscCtrls osc1, osc2;
        FltCtrls flt1;
    } SynthCtrls;

	typedef enum SynthCtrlId {
		// envelopes
		amAdsrAmp, amAdsrDc, amAdsrAtk, amAdsrDec, amAdsrSus, amAdsrRel,
		fmAdsrAmp, fmAdsrDc, fmAdsrAtk, fmAdsrDec, fmAdsrSus, fmAdsrRel,
		pmAdsrAmp, pmAdsrDc, pmAdsrAtk, pmAdsrDec, pmAdsrSus, pmAdsrRel,
		ftAdsrAmp, ftAdsrDc, ftAdsrAtk, ftAdsrDec, ftAdsrSus, ftAdsrRel,
		// LFOs
		amLfoAmp, amLfoFre,
		fmLfoAmp, fmLfoFre,
		// oscillators
		osc1Amp, osc1Fre, osc1Note, osc1Tune, osc1Psw, osc1Wave,
		osc2Amp, osc2Fre, osc2Note, osc2Tune, osc2Psw, osc2Wave,

		flt1Cut,/*=fmAdsrDc*/ flt1Res, flt1Mod,/*=fmAdsrAmp*/ flt1Mode
	} SynthCtrlId;

    #define SynthCtrlCount (sizeof(SynthCtrls)/sizeof(PotBase))

    class Synth : public SynthBase {
		static Soundbank* defaultSoundbank_;
		// Voice handling
		VOICEHANDLER setupVoiceHandler;
		VOICERENDERER renderVoiceHandler;
		VOICEHANDLER freeVoiceHandler;
		VOICESETNOTEHANDLER setNoteVoiceHandler;
    public:
        Synth(float* samplingRate, int voiceCount = 1);
        virtual ~Synth();

		SynthCtrls controls;

        // Module
		Soundbank* getDefaultSoundbank();

		Stream* writeProgramToStream();

		static void prepare();
		static void cleanUp();
    };
}

#endif
