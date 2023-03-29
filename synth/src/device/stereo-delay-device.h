#ifndef __DELAY_DEVICE_H
#define __DELAY_DEVICE_H

#include "synth/src/device/synth-adapter.h"
#include "synth/src/device/module-device.h"
#include "synth/src/module/stereo-delay.h"


using namespace PLAYER;
namespace SYNTH {

	class StereoDelayDevice : public ModuleDevice {
	public:
		StereoDelayDevice(SynthAdapter* adapter);
		~StereoDelayDevice();

#ifdef PLAYER_EDIT_MODE
		int writeToStream(Stream* stream);
#endif
	};
}
#endif