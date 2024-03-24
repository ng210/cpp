#ifndef _SYNTH_DEVICE_EXT_H
#define _SYNTH_DEVICE_EXT_H

#include "synth/src/device/synth-base-device-ext.h"
#include "synth/src/device/synth-device.h"

using namespace PLAYER;
namespace SYNTH {

	class SynthDeviceExt : public SynthBaseDeviceExt {
		static DeviceExt* synthDeviceExtCreator_(Device* device);
	public:
		SynthDeviceExt(Device* device);

		Sequence* createDefaultSequence();

		static void registerExtensionCreator();
	};
}

#endif