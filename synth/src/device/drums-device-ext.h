#ifndef _BASS_DEVICE_EXT_H
#define _BASS_DEVICE_EXT_H

#include "synth/src/device/synth-base-device-ext.h"
#include "synth/src/device/bass-device.h"

using namespace PLAYER;
namespace SYNTH {

	class BassDeviceExt : public SynthBaseDeviceExt {
		static DeviceExt* bassDeviceExtCreator_(Device* device);
	public:
		BassDeviceExt(Device* device);

		Sequence* createDefaultSequence();

		static void registerExtensionCreator();
	};
}

#endif