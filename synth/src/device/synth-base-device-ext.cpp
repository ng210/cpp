#include "synth/src/device/synth-base-device.h"
#include "synth/src/device/synth-base-device-ext.h"

using namespace SYNTH;
using namespace PLAYER;


SynthBaseDeviceExt::SynthBaseDeviceExt(Device* device) : ModuleDeviceExt(device) {}

void SynthBaseDeviceExt::writeToStream(Stream* stream) {
	ModuleDeviceExt::writeToStream(stream);
	var dev = (SynthBaseDevice*)device_;
	stream->writeByte(dev->synthBase()->voiceCount());
}
