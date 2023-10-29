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

    typedef struct BassCtrlsPots {
    } BassCtrlsPots;

    typedef struct BassCtrls {
        AdsrCtrls amAdsr, pmAdsr, ftAdsr;
        OscCtrls osc1, osc2;
        FltCtrls flt1;
        BassCtrlsPots pots;
    } BassCtrls;

    enum class BassCtrlId {
        // envelopes
        amAdsrAmp, amAdsrAtk, amAdsrDec, amAdsrSus, amAdsrRel,
        pmAdsrAmp, pmAdsrAtk, pmAdsrDec, pmAdsrSus, pmAdsrRel,
        cmAdsrAmp, cmAdsrAtk, cmAdsrDec, cmAdsrSus, cmAdsrRel,
        // oscillators
        osc1Amp, osc1Fre, osc1Note, osc1Tune, osc1Psw, osc1Wave,
        osc2Amp, osc2Fre, osc2Note, osc2Tune, osc2Psw, osc2Wave,
        // filter
        flt1Cut, flt1Res, flt1Mode
    };

#define BassCtrlCount (sizeof(BassCtrls)/sizeof(PotBase))

    class Bass : public SynthBase {
        static Soundbank* defaultSoundbank_;
        // Voice handling
        VOICEHANDLER setupVoiceHandler;
        VOICERENDERER renderVoiceHandler;
        VOICEHANDLER freeVoiceHandler;
        VOICESETNOTEHANDLER setNoteVoiceHandler;

    protected: BassCtrls controls;
    public:
        Bass(int voiceCount = 1);
        virtual ~Bass();

        // Module
        Soundbank* getDefaultSoundbank();

        static void prepare();
        static void cleanUp();
    };
}

#endif
