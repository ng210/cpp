#ifndef __MIXER_H
#define __MIXER_H

#include "synth/src/module/module.h"

NS_FW_BASE_USE
namespace SYNTH {

    #pragma region Typedefs
	typedef enum MixerCtrlId {
        MxCh1amp, MxCh1pan, MxCh1gain, MxCh1st1gain, MxCh1st2gain, MxCh1st3gain, MxCh1st4gain,
        MxCh2amp, MxCh2pan, MxCh2gain, MxCh2st1gain, MxCh2st2gain, MxCh2st3gain, MxCh2st4gain,
        MxCh3amp, MxCh3pan, MxCh3gain, MxCh3st1gain, MxCh3st2gain, MxCh3st3gain, MxCh3st4gain,
        MxCh4amp, MxCh4pan, MxCh4gain, MxCh4st1gain, MxCh4st2gain, MxCh4st3gain, MxCh4st4gain,
        MxCh5amp, MxCh5pan, MxCh5gain, MxCh5st1gain, MxCh5st2gain, MxCh5st3gain, MxCh5st4gain,
        MxCh6amp, MxCh6pan, MxCh6gain, MxCh6st1gain, MxCh6st2gain, MxCh6st3gain, MxCh6st4gain,
        MxCh7amp, MxCh7pan, MxCh7gain, MxCh7st1gain, MxCh7st2gain, MxCh7st3gain, MxCh7st4gain,
        MxCh8amp, MxCh8pan, MxCh8gain, MxCh8st1gain, MxCh8st2gain, MxCh8st3gain, MxCh8st4gain
	} MixerCtrlId;

    #define MixerChCtrlCount (MxCh2amp - MxCh1amp)

    typedef struct MixerStageCtrls_ {
        PotF8 gain;
    } MixerStageCtrls;

    typedef struct MixerChannelCtrls_ {
        PotF8 amp;
        PotF8 pan;
        PotF8 gain;
        MixerStageCtrls stages[4];
    } MixerChannelCtrls;

    typedef struct MixerCtrls_ {
        MixerChannelCtrls channels[8];
    } MixerCtrls;

    #define MixerCtrlsCount (sizeof(MixerCtrls) / sizeof(PotBase))

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
    #pragma endregion

    class Mixer8x4 : public Module {
        MixerChannel channels_[8];

        static int frame_;
        static float buffer_[SAMPLE_BUFFER_SIZE];
        static Soundbank* defaultSoundbank_;
    public:
        PROP(int, channelCount);
        Mixer8x4();
        virtual ~Mixer8x4();

        MixerCtrls controls;

        void initializeFromStream(byte** pData);
        void run(float* buffer, int start, int end);

        MixerChannel* connectInput(MixerChannel* channel, Module* input);
        MixerChannel* connectEffect(MixerChannel* channel, Module* effect, int stageId = -1);
        MixerChannel* getChannel(int id);

        Soundbank* getDefaultSoundbank();

        static void fillSoundBuffer(short* buffer, int bufferSize, void* args);
    };
}
#endif
