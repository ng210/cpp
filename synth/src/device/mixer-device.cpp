#include "soundlib/src/soundplayer.h"
#include "base/memory.h"
#include "player/src/player.h"
#include "synth/src/device/mixer-device.h"
#include "synth/src/module/mixer.h"

using namespace SYNTH;

MixerDevice::MixerDevice(SynthAdapter* adapter, Player* player) : ModuleDevice(adapter, player, NEW_(Mixer8x4)) {
	type(SynthDevices::DeviceMixer);
	inputs_ = (InputBase*)&mixerInputs;
	inputCount_ = sizeof(MixerInputs) / sizeof(InputBase);
	for (var ci = 0; ci < 8; ci++) {
		MixerChannelInputs& ch = mixerInputs.channels[ci];
		ch.amp.setup(0, 255, 1);
		ch.pan.setup(0, 255, 1);
		ch.gain.setup(0, 255, 1);
		for (var si = 0; si < 4; si++) {
			MixerStageInputs& st = ch.stages[si];
			st.gain.setup(0, 255, 1);
		}
	}
	assignInputs();
	frame_ = 0;
	samplePerFrame_ = 48000.f;
}
MixerDevice::~MixerDevice() {
	DEL_((Mixer8x4*)object_);
}

Mixer8x4* MixerDevice::mixer() {
	return (Mixer8x4*)object_;
}

void MixerDevice::initialize(byte** pData) {
	if (pData != NULL && *pData != NULL) {
		// 00 01 preset id
		// 01 01 channel count
		// 02 01 channel#1 input device id
		// 03 01 channel#1 stage count
		// 04 01 channel#1.stage#1 effect device id
		// ...
		// 07 01 channel#1.stage#4 effect device id
		// 08 01 channel#2 input device id
		// ...
		ModuleDevice::initialize(pData);
		var mixer = (Mixer8x4*)object_;
		var chnCount = READ(*pData, byte);
		if (mixer->channelCount() != chnCount) {
			mixer->channelCount(chnCount);
		}
		for (var ci = 0; ci < chnCount; ci++) {
			var ch = mixer->getChannel(ci);
			// read input device index
			var di = READ(*pData, byte);
			var device = (Device*)player_->devices().get(di);
			mixer->connectInput(ch, (Module*)device->object());
			
			var stCount = READ(*pData, byte);
			for (var si = 0; si < stCount; si++) {
				// read stage effect device index
				di = READ(*pData, byte);
				device = (Device*)player_->devices().get(di);
				mixer->connectEffect(ch, (Module*)device->object(), si);
			}
		}
	}
	//samplePerFrame_ = samplePerSecond/framePerSecond
	samplePerFrame_ = ((SynthAdapter*)adapter_)->samplingRate / player_->refreshRate();
	frame_ = 0;
}

int MixerDevice::getPresetBankSize() {
	return MixerInputSize;
}

PresetBank* MixerDevice::getDefaultPresetBank() {
	return NULL;
}