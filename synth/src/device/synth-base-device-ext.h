#ifndef __SYNTH_BASE_DEVICE_EXT_H
#define __SYNTH_BASE_DEVICE_EXT_H

#include "synth/src/device/module-device-ext.h"

using namespace PLAYER;
namespace SYNTH {

	class SynthBaseDeviceExt : public ModuleDeviceExt
	{
	protected:
	public:
		SynthBaseDeviceExt(Device* device);

		void writeToStream(Stream* stream);
	};
}
#endif