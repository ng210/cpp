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
	};
}
#endif