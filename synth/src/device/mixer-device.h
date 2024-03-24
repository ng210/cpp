#ifndef __MIXER_DEVICE_H
#define __MIXER_DEVICE_H

#include "collection/parray.h"
#include "synth/src/device/synth-adapter.h"
#include "synth/src/device/module-device.h"
#include "synth/src/module/mixer.h"

NS_FW_BASE_USE
using namespace PLAYER;
namespace SYNTH {

	typedef struct MixerStageInputs_ {
		InputF8 gain;
	} MixerStageInputs;
	#define MixerStageInputSize (1)

	typedef struct MixerChannelInputs_ {
		InputF8 amp, pan, gain;
		MixerStageInputs stages[4];
	} MixerChannelInputs;
	#define MixerChannelInputSize (4*MixerStageInputSize + 3)

	typedef struct MixerInputs_ {
		MixerChannelInputs channels[8];
	} MixerInputs;
	#define MixerInputSize (8*MixerChannelInputSize)

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

	class MixerDevice : public ModuleDevice {
	protected: PROP(int, frame);
	protected: PROP(float, samplePerFrame);

	public:
		MixerInputs mixerInputs;

		MixerDevice(SynthAdapter* adapter, Player* player);
		~MixerDevice();

		Mixer8x4* mixer();

		void initialize(byte** stream = NULL);
		//int run(int ticks);
		//void processCommand(byte cmd, byte*& cursor);
		int getPresetBankSize();
		PresetBank* getDefaultPresetBank();
	};
}
#endif