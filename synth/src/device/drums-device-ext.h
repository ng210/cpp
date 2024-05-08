#ifndef _DRUMS_DEVICE_EXT_H
#define _DRUMS_DEVICE_EXT_H

#include "synth/src/device/module-device-ext.h"
#include "synth/src/device/drums-device.h"

using namespace PLAYER;
namespace SYNTH {

	class DrumsDeviceExt : public ModuleDeviceExt {
		static DeviceExt* drumsDeviceExtCreator_(Device* device);
	public:
		DrumsDeviceExt(Device* device);

		Sequence* createDefaultSequence();

		static void registerExtensionCreator();
	};
}

#endif