#ifndef __MIXER_H
#define __MIXER_H

#include "synth/src/module/module.h"
#include "synth/src/elem/elem.h"

NS_FW_BASE_USE
namespace SYNTH {

    typedef struct MixerStageValues_ {
        Value gain;
    } MixerStageValues;

    typedef struct MixerChannelValues_ {
        Value amp, pan, gain;
        MixerStageValues stages[4];
    } MixerChannelValues;

    typedef struct MixerValues_ {
        MixerChannelValues channels[8];
    } MixerValues;

    typedef struct MixerStage_ {
        //MixerStageValues* values;
        Module* effect;
    } MixerStage;

    typedef struct MixerChannel_ {
        //MixerChannelValues* values;
        Module* input;
        //float* buffer;
        int stageCount;
        MixerStage stages[4];
    } MixerChannel;

    class Mixer8x4 : public Module {
        
        static int frame_;
        static float buffer_[SAMPLE_BUFFER_SIZE];

    protected:
        MixerValues values_;
        MixerChannel channels_[8];
    protected: PROP(int, channelCount);
    public:

        Mixer8x4();
        ~Mixer8x4();

        Value* getValues();

        // routing
        //void connectInput(int id, float* buffer);
        MixerChannel* connectInput(int chnId, Module* input, float amp = 1.0f, byte pan = 128, byte gain = 200);
        MixerChannel* connectEffect(int chnId, Module* effect, int gain = 80, int stageId = -1);
        MixerChannel* getChannel(int id);
        MixerChannel* setupChannel(int chnId, float amp, byte pan, byte gain);

        //void run(float* buffer, int start, int end);

        static short* fillSoundBuffer(short* buffer, int bufferSize, void* args);
    };
}
#endif
