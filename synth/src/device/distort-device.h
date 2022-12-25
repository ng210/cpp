#ifndef __CLIP_DEVICE_H
#define __CLIP_DEVICE_H

#include "synth-adapter.h"
#include "module-device.h"

using namespace PLAYER;
namespace SYNTH {

	class DistortDevice: public ModuleDevice {
	public:
		DistortDevice(SynthAdapter* adapter);
		~DistortDevice();
	};
}
#endif