#ifndef __CLIP_DEVICE_H
#define __CLIP_DEVICE_H

#include "synth/src/device/synth-adapter.h"
#include "synth/src/device/module-device.h"
#include "synth/src/module/distort.h"

using namespace PLAYER;
namespace SYNTH {

	class DistortDevice: public ModuleDevice {
	public:
		DistortDevice(SynthAdapter* adapter);
		~DistortDevice();

#ifdef PLAYER_EDIT_MODE
		int writeToStream(Stream* stream);
#endif

	};
}
#endif