#include "soundlib/src/soundplayer.h"
#include "base/memory.h"
#include "player/src/player.h"
#include "synth/src/device/bass-device.h"

using namespace SYNTH;

#pragma region Creation
BassDevice::BassDevice(SynthAdapter* adapter) : SynthBaseDevice(adapter) {
	type(SynthDevices::DeviceBass);
	object_ = NEW_(Bass, &((SynthAdapter*)adapter_)->samplingRate, 1);
}

BassDevice::~BassDevice() {
	if (object_ != NULL) {
		DEL_((Bass*)object_);
		object_ = NULL;
	}
}
