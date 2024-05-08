#include "base/memory.h"
#include "player/src/player-ext.h"
#include "synth/src/device/stereo-delay-device-ext.h"

using namespace PLAYER;
using namespace SYNTH;

DeviceExt* StereoDelayDeviceExt::stereoDelayDeviceExtCreator_(Device* device) {
	DeviceExt* deviceExt = NULL;
	switch (device->type()) {
		case SynthDevices::DeviceStereoDelay: deviceExt = NEW_(StereoDelayDeviceExt, device);
	}
	return deviceExt;
}

StereoDelayDeviceExt::StereoDelayDeviceExt(Device* device) : ModuleDeviceExt(device) {
}

Sequence* StereoDelayDeviceExt::createDefaultSequence() {
	var seq = NEW_(Sequence);
	return seq;
}

void StereoDelayDeviceExt::registerExtensionCreator() {
	SynthAdapter adapter;
	var key = (adapter.getInfo()->id << 8) + SynthDevices::DeviceStereoDelay;
	PlayerExt::addDeviceExtension(key, StereoDelayDeviceExt::stereoDelayDeviceExtCreator_);
}
