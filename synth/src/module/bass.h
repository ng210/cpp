/******************************************************************************
 * PSynth library
 *****************************************************************************/

#ifndef __BASS_H
#define __BASS_H
#include "collection/array.h"
#include "synth/src/module/synth-base.h"
#include "synth/src/module/synth-def.h"

NS_FW_BASE_USE
namespace SYNTH {

    typedef struct BassCtrls {
        EnvCtrls amEnv, pmEnv, ftEnv;
        OscCtrls osc1, osc2;
        FltCtrls flt1;
    } BassCtrls;

    typedef enum BassCtrlId {
        // envelopes
        bAmEnvAmp, bAmEnvDc, bAmEnvAtk, bAmEnvDec, bAmEnvSus, bAmEnvRel,
        bPmEnvAmp, bPmEnvDc, bPmEnvAtk, bPmEnvDec, bPmEnvSus, bPmEnvRel,
        bFtEnvAmp, bFtEnvDc, bFtEnvAtk, bFtEnvDec, bFtEnvSus, bFtEnvRel,
        // oscillators
        bOsc1Amp, bOsc1Fre, bOsc1Note, bOsc1Tune, bOsc1Psw, bOsc1Wave,
        bOsc2Amp, bOsc2Fre, bOsc2Note, bOsc2Tune, bOsc2Psw, bOsc2Wave,

        bFlt1Cut,/*=fmEnvDc*/ bFlt1Res, bFlt1Mod,/*=fmEnvAmp*/ bFlt1Mode
    } BassCtrlId;

#define BassCtrlCount (sizeof(BassCtrls)/sizeof(Pot*))

    class Bass : public SynthBase {
    protected: BassCtrls controls_;
        // Voice handling
        VOICEHANDLER setupVoiceHandler;
        VOICERENDERER renderVoiceHandler;
        VOICEHANDLER freeVoiceHandler;
        VOICESETNOTEHANDLER setNoteVoiceHandler;

    public:
        Bass(float* samplingRate, int voiceCount = 1);
        virtual ~Bass();

        inline BassCtrls* controls() { return &controls_; };

        // Module
        void initialize(byte** pData);
    };
}

#endif
