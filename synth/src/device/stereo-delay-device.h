#ifndef __DELAY_DEVICE_H
#define __DELAY_DEVICE_H

#include "synth-adapter.h"
#include "module-device.h"
#include "../module/stereo-delay.h"


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