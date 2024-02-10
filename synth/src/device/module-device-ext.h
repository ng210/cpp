#ifndef __MODULE_DEVICE_EXT_H
#define __MODULE_DEVICE_EXT_H

#include "player/src/device-ext.h"
#include "synth/src/device/module-device.h"

NS_FW_BASE_USE
using namespace PLAYER;
namespace SYNTH {

	class ModuleDeviceExt : public DeviceExt {
	public:
		ModuleDeviceExt(Device* device);

		void makeCommandImpl(int command, Stream* stream, va_list args);
		int getCommandSize(byte* cmd);

		void writeToStream(Stream* stream);
	};
}
#endif