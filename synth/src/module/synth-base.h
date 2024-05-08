#ifndef __SYNTH_BASE_H
#define __SYNTH_BASE_H

#include "synth/src/module/module.h"
#include "synth/src/module/voice.h"

NS_FW_BASE_USE
namespace SYNTH {

	class SynthBase : public Module {
    protected: PROP_R(int, voiceCount);
    protected: Voice voices_[2*32];
    protected: VoiceUnit voiceUnits_[32];
    protected:
        // Voice handling
        LpSetupVoiceHandler setupVoiceHandler;
        LpSetNoteVoiceHandler setNoteVoiceHandler;
        LpVoiceHandler freeVoiceHandler;
        LpVoiceHandler killVoiceHandler;
        LpVoiceRenderer renderVoiceHandler;

    public:
        SynthBase();
        ~SynthBase();

        void initialize(int voiceCount);
        void voiceCount(int count);

        void setNote(byte note, byte velocity);

        // Module
        void connectInput(int id, float* buffer);
        bool isActive();
        float* getOutput(int id);
        void run(int start, int end, BufferWriteModes mode = BufferWriteModes::Overwrite);
	};
}
#endif
