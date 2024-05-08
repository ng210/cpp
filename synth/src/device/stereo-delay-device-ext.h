#ifndef _DELAY_DEVICE_EXT_H
#define _DELAY_DEVICE_EXT_H

#include "synth/src/device/module-device-ext.h"
#include "synth/src/device/stereo-delay-device.h"

using namespace PLAYER;
namespace SYNTH {

	class StereoDelayDeviceExt : public ModuleDeviceExt {
		static DeviceExt* stereoDelayDeviceExtCreator_(Device* device);
	public:
		StereoDelayDeviceExt(Device* device);

		Sequence* createDefaultSequence();

		static void registerExtensionCreator();
	};
}

#endif