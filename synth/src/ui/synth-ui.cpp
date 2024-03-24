#include "synth/src/ui/synth-ui.h"

using namespace SYNTH;

DeviceCtrl* SynthUI::synthControlCreator_(DeviceExt* deviceExt) {
	DeviceCtrl* deviceCtrl = NULL;
	switch (deviceExt->device()->type()) {
		case SynthDevices::DeviceBass: deviceCtrl = NEW_(BassCtrl, deviceExt); break;
		case SynthDevices::DeviceSynth: deviceCtrl = NEW_(SynthCtrl, deviceExt); break;
		case SynthDevices::DeviceMixer: deviceCtrl = NEW_(MixerCtrl, deviceExt); break;
	}
	return deviceCtrl;
}

void SynthUI::registerControlCreators() {
	SynthAdapter adapter;
	var key = (adapter.getInfo()->id << 8) + SynthDevices::DeviceBass;
	DeviceCtrl::addDeviceControl(key, SynthUI::synthControlCreator_);
	key = (adapter.getInfo()->id << 8) + SynthDevices::DeviceSynth;
	DeviceCtrl::addDeviceControl(key, SynthUI::synthControlCreator_);
	key = (adapter.getInfo()->id << 8) + SynthDevices::DeviceMixer;
	DeviceCtrl::addDeviceControl(key, SynthUI::synthControlCreator_);
}
