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
        Pot velocity;
        Pot note;
        Adsr* envelopes;
        Lfo* lfos;
        Osc* oscillators;
        Flt* filters;
        PotBase** pots;
        Voice() {
            envelopes = NULL;
            lfos = NULL;
            oscillators = NULL;
            filters = NULL;
            pots = NULL;
        }
    } Voice;

    class SynthBase;
    typedef void(VOICEHANDLER)(Voice& v);
    typedef void(SynthBase::*PVOICEHANDLER)(Voice& v);
    typedef void(VOICESETNOTEHANDLER)(Voice& v, byte note, byte velocity);
    typedef void(SynthBase::*PVOICESETNOTEHANDLER)(Voice& v, byte note, byte velocity);
    typedef void(VOICERENDERER)(Voice& v, float* buffer, int start, int end);
    typedef void(SynthBase::*PVOICERENDERER)(Voice& v, float* buffer, int start, int end);

	class SynthBase : public Module {
    protected: PROP_R(int, voiceCount);
    protected: Voice voices_[32];
    protected:
        // Voice handling
        PVOICEHANDLER setupVoice;
        PVOICEHANDLER freeVoice;
        PVOICESETNOTEHANDLER setNoteVoice;
        PVOICERENDERER renderVoice;
    public:
        SynthBase(PotBase* controls, int count);
        ~SynthBase();

        void initialize(int voiceCount);
        void setNote(byte note, byte velocity);
        void voiceCount(int count);

        // Module
        void connectInput(int id, float* buffer);
        bool isActive();
        inline float* getOutput(int id);
        void run(int start, int end);
	};
}
#endif
