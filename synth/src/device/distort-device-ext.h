#ifndef _DISTORT_DEVICE_EXT_H
#define _DISTORT_DEVICE_EXT_H

#include "synth/src/device/synth-base-device-ext.h"
#include "synth/src/device/distort-device.h"

using namespace PLAYER;
namespace SYNTH {

	class DistortDeviceExt : public SynthBaseDeviceExt {
		static DeviceExt* distortDeviceExtCreator_(Device* device);
	public:
		DistortDeviceExt(Device* device);

		Sequence* createDefaultSequence();

		static void registerExtensionCreator();
	};
}

#endif