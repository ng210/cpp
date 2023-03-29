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
        AdsrCtrls amAdsr, pmAdsr, ftAdsr;
        OscCtrls osc1, osc2;
        FltCtrls flt1;
    } BassCtrls;

    typedef enum BassCtrlId {
        // envelopes
        bAmAdsrAmp, bAmAdsrDc, bAmAdsrAtk, bAmAdsrDec, bAmAdsrSus, bAmAdsrRel,
        bPmAdsrAmp, bPmAdsrDc, bPmAdsrAtk, bPmAdsrDec, bPmAdsrSus, bPmAdsrRel,
        bFtAdsrAmp, bFtAdsrDc, bFtAdsrAtk, bFtAdsrDec, bFtAdsrSus, bFtAdsrRel,
        // oscillators
        bOsc1Amp, bOsc1Fre, bOsc1Note, bOsc1Tune, bOsc1Psw, bOsc1Wave,
        bOsc2Amp, bOsc2Fre, bOsc2Note, bOsc2Tune, bOsc2Psw, bOsc2Wave,

        bFlt1Cut,/*=fmAdsrDc*/ bFlt1Res, bFlt1Mod,/*=fmAdsrAmp*/ bFlt1Mode
    } BassCtrlId;

#define BassCtrlCount (sizeof(BassCtrls)/sizeof(PotBase))

    class Bass : public SynthBase {
        static Soundbank* defaultSoundbank_;
        // Voice handling
        VOICEHANDLER setupVoiceHandler;
        VOICERENDERER renderVoiceHandler;
        VOICEHANDLER freeVoiceHandler;
        VOICESETNOTEHANDLER setNoteVoiceHandler;

    public:
        Bass(float* samplingRate, int voiceCount = 1);
        virtual ~Bass();

        BassCtrls controls;

        // Module
        Soundbank* getDefaultSoundbank();

        static void prepare();
        static void cleanUp();
    };
}

#endif
