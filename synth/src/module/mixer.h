#ifndef __MIXER_H
#define __MIXER_H

#include "module.h"
#include "mixer-controls.h"

NS_FW_BASE_USE
namespace SYNTH {

    typedef struct MixerStageCtrls_ {
        PotF8 blend;
        PotF8 gain;
    } MixerStageCtrls;

    typedef struct MixerChannelCtrls_ {
        PotF8 gain;
        Pot pan;
        PotF8 amp;
        MixerStageCtrls stages[4];
    } MixerChannelCtrls;

    typedef struct MixerCtrls_ {
        MixerChannelCtrls channels[8];
    } MixerCtrls;

#define MixerCtrlsCount (sizeof(MixerCtrls) / sizeof(Pot*))

    class Effect;
    class Mixer8x4;

    typedef struct MixerStage_ {
        MixerStageCtrls* controls;
        Module* effect;
    } MixerStage;

    typedef struct MixerChannel_ {
        MixerChannelCtrls* controls;
        Module* input;
        //float* buffer;
        int stageCount;
        MixerStage stages[4];
    } MixerChannel;

    class Mixer8x4 : public Module {
    private:
        MixerChannel channels_[8];
        MixerCtrls controls_;
        //float* combinedBuffer_;

        static int frame_;
        static float buffer_[SAMPLE_BUFFER_SIZE];
    public:
        PROP(int, channelCount);
        Mixer8x4();
        virtual ~Mixer8x4();

        void initialize(byte** pData);
        void run(float* buffer, int start, int end);

        MixerChannel* connectInput(MixerChannel* channel, Module* input);
        MixerChannel* connectEffect(MixerChannel* channel, Module* effect, int stageId = -1);
        MixerChannel* getChannel(int id);

        static void fillSoundBuffer(short* buffer, int bufferSize, void* args);
    };
}
#endif
