#ifndef _MIXER_DEVICE_EXT_H
#define _MIXER_DEVICE_EXT_H

#include "synth/src/device/module-device-ext.h"
#include "synth/src/device/mixer-device.h"

using namespace PLAYER;
namespace SYNTH {

	class MixerDeviceExt : public ModuleDeviceExt {
		static DeviceExt* mixerDeviceExtCreator_(Device* device);
	public:
		MixerDeviceExt(Device* device);

		void writeToStream(Stream* stream);

		static void registerExtensionCreator();
	};
}

#endif