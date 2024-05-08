#ifndef __DELAY_DEVICE_H
#define __DELAY_DEVICE_H

#include "synth/src/device/synth-adapter.h"
#include "synth/src/device/module-device.h"
#include "synth/src/module/stereo-delay.h"


using namespace PLAYER;
namespace SYNTH {

	typedef struct StereoDelayInputs_ {
		DlyInputs left;
		InputF8 mixLeft;
		DlyInputs right;
		InputF8 mixRight;
		FltInputs flt;
	} StereoDelayInputs;
	#define StereoDelayInputSize (2*DlyInputSize + 2 + FltInputSize)

	typedef enum StereoDelayInputIds {
		stdlFeedbackLeft,
		stdlDelayLeft,
		stdlMixLeft,
		stdlFeedbackRight,
		stdlDelayRight,
		stdlMixRight,
		stdlCut,
		stdlRes,
		stdlMode
	} StereoDelayInputIds;

	class StereoDelayDevice : public ModuleDevice {
	protected: StereoDelayInputs stereoDelayInputs;
	public:		
		StereoDelayDevice(SynthAdapter* adapter, Player* player);
		~StereoDelayDevice();

		int getPresetBankSize();
		PresetBank* getDefaultPresetBank();

		static int delaySetter(void*, Value, void* = NULL);
	};
}

#endif