#ifndef _BASS_DEVICE_EXT_H
#define _BASS_DEVICE_EXT_H

#include "synth/src/device/module-device-ext.h"

using namespace PLAYER;
namespace SYNTH {

	class BassDeviceExt : public ModuleDeviceExt {
		static DeviceExt* bassDeviceExtCreator_(Device* device);
	public:
		BassDeviceExt(Device* device);

		void setupInputs();

		Sequence* createDefaultSequence();

		static void registerExtensionCreator();
	};
}

#endif