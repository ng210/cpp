#include "base/memory.h"
#include "player/src/player-ext.h"
#include "synth/src/device/generic-drum-device-ext.h"

using namespace PLAYER;
using namespace SYNTH;

DeviceExt* GenericDrumDeviceExt::genericDrumDeviceExtCreator_(Device* device) {
	DeviceExt* deviceExt = NULL;
	switch (device->type()) {
		case SynthDevices::DeviceGenericDrum: deviceExt = NEW_(GenericDrumDeviceExt, device);
	}
	return deviceExt;
}

GenericDrumDeviceExt::GenericDrumDeviceExt(Device* device) : ModuleDeviceExt(device) {
}

Sequence* GenericDrumDeviceExt::createDefaultSequence() {
	return NULL;
}

void GenericDrumDeviceExt::registerExtensionCreator() {
	SynthAdapter adapter;
	var key = (adapter.getInfo()->id << 8) + SynthDevices::DeviceGenericDrum;
	PlayerExt::addDeviceExtension(key, GenericDrumDeviceExt::genericDrumDeviceExtCreator_);
}
