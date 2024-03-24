#ifndef __VOICE_H
#define __VOICE_H

#include "synth/src/elem/adsr.h"
#include "synth/src/elem/dahr.h"
#include "synth/src/elem/lfo.h"
#include "synth/src/elem/osc.h"
#include "synth/src/elem/flt.h"

NS_FW_BASE_USE
namespace SYNTH {
    class Voice;
    class SynthBase;
    typedef void(VoiceHandler)(Voice* v);
    typedef VoiceHandler* LpVoiceHandler;
    typedef void(SetupVoiceHandler)(Voice* v, Value* values);
    typedef SetupVoiceHandler* LpSetupVoiceHandler;
    typedef void(SetNoteVoiceHandler)(Voice* v, byte note, byte velocity);
    typedef SetNoteVoiceHandler* LpSetNoteVoiceHandler;
    typedef void(VoiceRenderer)(Voice* v, SynthBase* synth, float* buffer, int start, int end);
    typedef VoiceRenderer* LpVoiceRenderer;

    class Voice {
    public:
        Value velocity;
        Value note;
        int envCount;
        Env* envelopes;
        int lfoCount;
        Lfo* lfos;
        int oscCount;
        Osc* oscillators;
        int fltCount;
        Flt* filters;
        Value* values;

        Voice();

        static SetNoteVoiceHandler setNoteVoiceDefaultHandler;
        static VoiceHandler freeVoiceDefaultHandler;
        static VoiceHandler killVoiceDefaultHandler;

        void setupVoiceAdsr(int count, AdsrValues* values);
        void setupVoiceDahr(int count, DahrValues* values);
        void setupVoiceLfo(int count, LfoValues* values);
        void setupVoiceOsc(int count, OscValues* values);
        void setupVoiceFlt(int count, FltValues* values);
    };

    class VoiceUnit {
        Voice* voices_[2];
    protected: PROP_R(int, activeVoiceId);
    public:
        VoiceUnit();
        ~VoiceUnit();
        void setVoices(Voice* a, Voice* b);

        Voice* activeVoice();
        Voice* swapVoices();
        bool matchVoiceForNoteOff(byte note);
        bool isActive();
        int ticks();
    };
}

#endif