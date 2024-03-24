#ifndef __GENERIC_DRUM_DEVICE_EXT_H
#define __GENERIC_DRUM_DEVICE_EXT_H

#include "synth/src/device/module-device-ext.h"
#include "synth/src/device/generic-drum-device.h"

using namespace PLAYER;
namespace SYNTH {

	class GenericDrumDeviceExt : public ModuleDeviceExt {
		static DeviceExt* genericDrumDeviceExtCreator_(Device* device);
	public:
		GenericDrumDeviceExt(Device* device);

		Sequence* createDefaultSequence();

		static void registerExtensionCreator();
	};
}

#endif