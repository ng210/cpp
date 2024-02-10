#include "synth/src/device/synth-base-device.h"
#include "synth/src/device/synth-base-device-ext.h"

using namespace SYNTH;
using namespace PLAYER;


SynthBaseDeviceExt::SynthBaseDeviceExt(Device* device) : ModuleDeviceExt(device) {}

void SynthBaseDeviceExt::writeToStream(Stream* stream) {
	var dev = (SynthBaseDevice*)device_;
	DeviceExt::writeToStream(stream);
	stream->writeByte(dev->synthBase()->voiceCount());
	ModuleDeviceExt::writeToStream(stream);
}
