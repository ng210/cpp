#ifndef __SYNTH_BASE_H
#define __SYNTH_BASE_H

#include "synth/src/module/module.h"
#include "synth/src/elem/adsr.h"
#include "synth/src/elem/lfo.h"
#include "synth/src/elem/osc.h"
#include "synth/src/elem/flt.h"

NS_FW_BASE_USE
namespace SYNTH {

    //typedef struct SynthBaseCtrls {
    //    PotF8 amp;
    //} SynthBaseCtrls;

    typedef struct Voice {
        Pot velocity;
        Pot note;
        Adsr* envelopes;
        Osc* oscillators;
        Lfo* lfos;
        Flt* filters;
        Voice() {
            envelopes = NULL;
            oscillators = NULL;
            lfos = NULL;
            filters = NULL;
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
    protected: PROP_R(float*, samplingRate);
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

        void initialize(float* samplingRate, int voiceCount);
        void samplingRate(float* smpRate);
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
