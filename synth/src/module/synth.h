/******************************************************************************
 * PSynth library
 *****************************************************************************/

#ifndef __SYNTH_H
#define __SYNTH_H

#include "collection/array.h"
#include "voice.h"
#include "module.h"
#include "synth-controls.h"

NS_FW_BASE_USE
namespace SYNTH {

    class Voice;

    typedef struct SynthCtrls_ {
        PotF8 amp;
        EnvCtrls env1;
        EnvCtrls env2;
        EnvCtrls env3;
        EnvCtrls env4;
        LfoCtrls lfo1;
        LfoCtrls lfo2;
        OscCtrls osc1;
        OscCtrls osc2;
        FltCtrls flt1;
    } SynthCtrls;

    typedef enum SynthPitch {
        pNone = 0,
             pCs0, pD0, pDs0, pE0, pF0, pFs0, pG0, pGs0, pA0, pAs0, pH0,
        pC1, pCs1, pD1, pDs1, pE1, pF1, pFs1, pG1, pGs1, pA1, pAs1, pH1,
        pC2, pCs2, pD2, pDs2, pE2, pF2, pFs2, pG2, pGs2, pA2, pAs2, pH2,
        pC3, pCs3, pD3, pDs3, pE3, pF3, pFs3, pG3, pGs3, pA3, pAs3, pH3,
        pC4, pCs4, pD4, pDs4, pE4, pF4, pFs4, pG4, pGs4, pA4, pAs4, pH4,
        pC5, pCs5, pD5, pDs5, pE5, pF5, pFs5, pG5, pGs5, pA5, pAs5, pH5
    } SynthPitch;

    #define SynthCtrlCount (sizeof(SynthCtrls)/sizeof(Pot*))

    class Synth : public Module {
    private: SynthCtrls controls_;
    private: PROP_R(float, samplingRate);
    private: double omega;
    private: Voice* nextVoice;
    private: PROP_R(Array, voices);
    private: PROP(byte*, soundBank);
    private: PROP(int, program);

    public:
        bool isActive;

        Synth(float* samplingRate, int voiceCount = 1);
        virtual ~Synth();

        void initialize(byte** pData);
        void connectInput(int id, float* buffer);
        inline float* getOutput(int id);
        void run(int start, int end);

        void initialize(float* samplingRate, int voiceCount);
        inline SynthCtrls controls() { return controls_;  }

        void samplingRate(float* smpRate);
        void setVoiceCount(int voiceCount);

        void setNote(byte note, byte velocity);
        void setProgram(int prgId);
    };
}

#endif
