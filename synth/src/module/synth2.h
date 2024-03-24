#ifndef __SYNTH2_H
#define __SYNTH2_H

#include "collection/array.h"
#include "synth/src/module/synth-base.h"
#include "synth/src/module/synth-def.h"

NS_FW_BASE_USE
namespace SYNTH {

	typedef struct Synth2CtrlsPots_ {
		PotF8 amOsc1, amOsc2;
		PotF fmOsc1, fmOsc2;
		PotF8 pmOsc1, pmOsc2; //, cmOsc1, cmOsc2;
		PotF8 flt1Amp, flt2Amp, flt3Amp;
		PotF8 flt1Band, flt2Band, flt3Band;
	} Synth2CtrlsPots;

	typedef struct Synth2Ctrls_ {
		AdsrCtrls amAdsr, fmAdsr, pmAdsr, cmAdsr;
		LfoCtrls amLfo, fmLfo, pmLfo, cmLfo;
		OscCtrls osc1, osc2;
		FltCtrls flt11, flt21, flt31, flt12, flt22, flt32;
		Synth2CtrlsPots pots;
	} Synth2Ctrls;

	typedef enum Synth2CtrlId {
		// envelopes
		amAdsrAmp, amAdsrAtk, amAdsrDec, amAdsrSus, amAdsrRel,
		fmAdsrAmp, fmAdsrAtk, fmAdsrDec, fmAdsrSus, fmAdsrRel,
		pmAdsrAmp, pmAdsrAtk, pmAdsrDec, pmAdsrSus, pmAdsrRel,
		cmAdsrAmp, cmAdsrAtk, cmAdsrDec, cmAdsrSus, cmAdsrRel,
		// LFOs
		amLfoAmp, amLfoFre, fmLfoAmp, fmLfoFre,
		pmLfoAmp, pmLfoFre, cmLfoAmp, cmLfoFre,
		// oscillators
		osc1Amp, osc1Fre, osc1Note, osc1Tune, osc1Psw, osc1Wave,
		osc2Amp, osc2Fre, osc2Note, osc2Tune, osc2Psw, osc2Wave,
		// filters
		flt11Cut, flt11Res, flt11Mode, flt12Cut, flt12Res, flt12Mode,
		flt21Cut, flt21Res, flt21Mode, flt22Cut, flt22Res, flt22Mode,
		flt31Cut, flt31Res, flt31Mode, flt32Cut, flt32Res, flt32Mode,
		// pots
		amOsc1, amOsc2, fmOsc1, fmOsc2, pmOsc1, pmOsc2, //cmOsc1, cmOsc2
		flt1Amp, flt2Amp, flt3Amp,
		flt1Band, flt2Band, flt3Band
	} Synth2CtrlId;

	#define Synth2CtrlCount (sizeof(Synth2Ctrls)/sizeof(PotBase))

    class Synth2 : public SynthBase {
		static Soundbank* defaultSoundbank_;
		// Voice handling
		VOICEHANDLER setupVoiceHandler;
		VOICERENDERER renderVoiceHandler;
		VOICEHANDLER freeVoiceHandler;
		VOICEHANDLER killVoiceHandler;
		VOICESETNOTEHANDLER setNoteVoiceHandler;

		//float applyFilter(Flt* st, float smp, float cut);

		static int cutoffSetter(void* obj1, S value, void* = NULL);

	public:
		Synth2(int voiceCount = 1);
		virtual ~Synth2();

		Synth2Ctrls controls;

		// Module
		Soundbank* getDefaultSoundbank();

		Stream* writeProgramToStream();

		static void prepare();
		static void cleanUp();
    };

}

#endif