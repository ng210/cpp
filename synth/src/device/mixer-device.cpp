#include "soundlib/src/soundplayer.h"
#include "base/memory.h"
#include "player/src/player.h"
#include "mixer-device.h"
#include "../module/mixer.h"

using namespace SYNTH;

MixerDevice::MixerDevice(SynthAdapter* adapter) : ModuleDevice(NEW_(Mixer8x4), (Adapter*)adapter) {
	frame_ = 0;
	type(SynthDevices::DeviceMixer);
}
MixerDevice::~MixerDevice() {
	DEL_((Mixer8x4*)object_);
}

#pragma region Device
void MixerDevice::initialize(byte** pData) {
	var mixer = (Mixer8x4*)object();
	if (pData != NULL && *pData != NULL) {
		((Module*)object())->initialize(pData);
		for (var ci = 0; ci < mixer->channelCount_; ci++) {
			var ch = mixer->getChannel(ci);
			var di = READ(*pData, byte);
			var device = (Device*)player_->devices().getAt(di);
			mixer->connectInput(ch, (Module*)device->object());
			for (var si = 0; si < ch->stageCount; si++) {
				di = READ(*pData, byte);
				device = (Device*)player_->devices().getAt(di);
				mixer->connectEffect(ch, (Module*)device->object(), si);
			}
		}
	}
	//samplePerFrame_ = samplePerSecond/framePerSecond
	samplePerFrame_ = ((SynthAdapter*)adapter_)->samplingRate / player_->refreshRate();
	frame_ = 0;
}

#pragma endregion

#ifdef PLAYER_EDIT_MODE
int MixerDevice::writeToStream(Stream* stream) {
	return Device::writeToStream(stream);
	// TODO!!!
}
#endif
