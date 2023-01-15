/******************************************************************************
 * PSynth library
 *****************************************************************************/

#ifndef __SYNTH_H
#define __SYNTH_H

#include "collection/array.h"
#include "synth/src/module/synth-base.h"
#include "synth/src/module/synth-def.h"

NS_FW_BASE_USE
namespace SYNTH {

    typedef struct SynthCtrls_ {
        EnvCtrls amEnv, fmEnv, pmEnv, ftEnv;
        LfoCtrls lfo1, lfo2;
        OscCtrls osc1, osc2;
        FltCtrls flt1;
    } SynthCtrls;

	typedef enum SynthCtrlId {
		// envelopes
		amEnvAmp, amEnvDc, amEnvAtk, amEnvDec, amEnvSus, amEnvRel,
		fmEnvAmp, fmEnvDc, fmEnvAtk, fmEnvDec, fmEnvSus, fmEnvRel,
		pmEnvAmp, pmEnvDc, pmEnvAtk, pmEnvDec, pmEnvSus, pmEnvRel,
		ftEnvAmp, ftEnvDc, ftEnvAtk, ftEnvDec, ftEnvSus, ftEnvRel,
		// LFOs
		amLfoAmp, amLfoFre,
		fmLfoAmp, fmLfoFre,
		// oscillators
		osc1Amp, osc1Fre, osc1Note, osc1Tune, osc1Psw, osc1Wave,
		osc2Amp, osc2Fre, osc2Note, osc2Tune, osc2Psw, osc2Wave,

		flt1Cut,/*=fmEnvDc*/ flt1Res, flt1Mod,/*=fmEnvAmp*/ flt1Mode
	} SynthCtrlId;

    #define SynthCtrlCount (sizeof(SynthCtrls)/sizeof(Pot*))

    class Synth : public SynthBase {
    protected: SynthCtrls controls_;
		// Voice handling
		VOICEHANDLER setupVoiceHandler;
		VOICERENDERER renderVoiceHandler;
		VOICEHANDLER freeVoiceHandler;
		VOICESETNOTEHANDLER setNoteVoiceHandler;
    public:
        Synth(float* samplingRate, int voiceCount = 1);
        virtual ~Synth();

        inline SynthCtrls* controls() { return &controls_; };

        // Module
        void initialize(byte** pData);
    };
}

#endif
