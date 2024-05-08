#include "base/memory.h"
#include "player/src/player-ext.h"
#include "synth/src/device/distort-device-ext.h"

using namespace PLAYER;
using namespace SYNTH;

DeviceExt* DistortDeviceExt::distortDeviceExtCreator_(Device* device) {
	DeviceExt* deviceExt = NULL;
	switch (device->type()) {
		case SynthDevices::DeviceDistort: deviceExt = NEW_(DistortDeviceExt, device);
	}
	return deviceExt;
}

DistortDeviceExt::DistortDeviceExt(Device* device) : SynthBaseDeviceExt(device) {
}

Sequence* DistortDeviceExt::createDefaultSequence() {
	return NULL;
}

void DistortDeviceExt::registerExtensionCreator() {
	SynthAdapter adapter;
	var key = (adapter.getInfo()->id << 8) + SynthDevices::DeviceDistort;
	PlayerExt::addDeviceExtension(key, DistortDeviceExt::distortDeviceExtCreator_);
}
