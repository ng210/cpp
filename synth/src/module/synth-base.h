#ifndef __SYNTH_BASE_H
#define __SYNTH_BASE_H

#include "synth/src/module/module.h"
#include "synth/src/elem/adsr.h"
#include "synth/src/elem/lfo.h"
#include "synth/src/elem/osc.h"
#include "synth/src/elem/flt.h"

NS_FW_BASE_USE
namespace SYNTH {


    typedef struct Voice {
        Value velocity;
        Value note;
        Adsr* envelopes;
        Lfo* lfos;
        Osc* oscillators;
        Flt* filters;
        Value* values;
        Voice() {
            envelopes = NULL;
            lfos = NULL;
            oscillators = NULL;
            filters = NULL;
            values = NULL;
        }
    } Voice;

    class SynthBase;
    typedef void(VoiceHandler)(Voice& v);
    typedef void(SynthBase::*LpVoiceHandler)(Voice& v);
    typedef void(VoiceSetNoteHandler)(Voice& v, byte note, byte velocity);
    typedef void(SynthBase::*LpVoiceSetNoteHandler)(Voice& v, byte note, byte velocity);
    typedef void(VoiceRenderer)(Voice& v, float* buffer, int start, int end);
    typedef void(SynthBase::*LpVoiceRenderer)(Voice& v, float* buffer, int start, int end);

	class SynthBase : public Module {
    protected: PROP_R(int, voiceCount);
    protected: Voice voices_[32];
    protected:
        // Voice handling
        LpVoiceHandler setupVoice;
        LpVoiceHandler freeVoice;
        LpVoiceSetNoteHandler setNoteVoice;
        LpVoiceRenderer renderVoice;
    public:
        SynthBase();
        ~SynthBase();

        void initialize(int voiceCount);
        void voiceCount(int count);

        void setNote(byte note, byte velocity);

        // Module
        void connectInput(int id, float* buffer);
        bool isActive();
        inline float* getOutput(int id);
        void run(int start, int end);
	};
}
#endif
