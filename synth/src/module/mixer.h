#ifndef __MIXER_H
#define __MIXER_H

#include "module.h"

NS_FW_BASE_USE
namespace SYNTH {

	typedef enum MixerCtrlId {
		// channel 0
		MCch1gain, MCch1pan, MCch1amp, MCch1st1blend, MCch1st1gain, MCch1st2blend, MCch1st2gain, MCch1st3blend, MCch1st3gain, MCch1st4blend, MCch1st4gain,
		// channel 1
        MCch2gain, MCch2pan, MCch2amp, MCch2st1blend, MCch2st1gain, MCch2st2blend, MCch2st2gain, MCch2st3blend, MCch2st3gain, MCch2st4blend, MCch2st4gain,
        // channel 2
        MCch3gain, MCch3pan, MCch3amp, MCch3st1blend, MCch3st1gain, MCch3st2blend, MCch3st2gain, MCch3st3blend, MCch3st3gain, MCch3st4blend, MCch3st4gain,
        // channel 3
        MCch4gain, MCch4pan, MCch4amp, MCch4st1blend, MCch4st1gain, MCch4st2blend, MCch4st2gain, MCch4st3blend, MCch4st3gain, MCch4st4blend, MCch4st4gain,
        // channel 4
        MCch5gain, MCch5pan, MCch5amp, MCch5st1blend, MCch5st1gain, MCch5st2blend, MCch5st2gain, MCch5st3blend, MCch5st3gain, MCch5st4blend, MCch5st4gain,
        // channel 5
        MCch6gain, MCch6pan, MCch6amp, MCch6st1blend, MCch6st1gain, MCch6st2blend, MCch6st2gain, MCch6st3blend, MCch6st3gain, MCch6st4blend, MCch6st4gain,
        // channel 6
        MCch7gain, MCch7pan, MCch7amp, MCch7st1blend, MCch7st1gain, MCch7st2blend, MCch7st2gain, MCch7st3blend, MCch7st3gain, MCch7st4blend, MCch7st4gain,
        // channel 7
        MCch8gain, MCch8pan, MCch8amp, MCch8st1blend, MCch8st1gain, MCch8st2blend, MCch8st2gain, MCch8st3blend, MCch8st3gain, MCch8st4blend, MCch8st4gain
	} MixerCtrlId;

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
