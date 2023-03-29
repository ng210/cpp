#include "soundlib/src/soundplayer.h"
#include "base/memory.h"
#include "player/src/player.h"
#include "synth/src/device/mixer-device.h"
#include "synth/src/module/mixer.h"

using namespace SYNTH;

MixerDevice::MixerDevice(SynthAdapter* adapter) : ModuleDevice(NEW_(Mixer8x4), (Adapter*)adapter) {
	frame_ = 0;
	samplePerFrame_ = 48000.f;
	type(SynthDevices::DeviceMixer);
}
MixerDevice::~MixerDevice() {
	DEL_((Mixer8x4*)object_);
}

#pragma region Device
void MixerDevice::initialize(byte** pData) {
	var mixer = (Mixer8x4*)object_;
	if (pData != NULL && *pData != NULL) {
		mixer->initializeFromStream(pData);
		for (var ci = 0; ci < mixer->channelCount_; ci++) {
			var ch = mixer->getChannel(ci);
			var di = READ(*pData, byte);
			var device = (Device*)player_->devices().get(di);
			mixer->connectInput(ch, (Module*)device->object());
			for (var si = 0; si < ch->stageCount; si++) {
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

#pragma endregion

#ifdef PLAYER_EDIT_MODE
int MixerDevice::writeToStream(Stream* stream) {
	var start = stream->cursor();
	Device::writeToStream(stream);
	var mx = (Mixer8x4*)object_;
	stream->writeByte(mx->channelCount_);
	// write channel/stage controls
	for (var i = 0; i < mx->channelCount_; i++) {
		var chn = mx->getChannel(i);
		stream->writeByte((byte)(chn->controls->gain.value.f * 255.f));
		stream->writeByte((byte)(chn->controls->pan.value.f * 100.0));
		stream->writeByte((byte)(chn->controls->amp.value.f * 255.f));
		stream->writeByte(chn->stageCount);
		for (var j = 0; j < chn->stageCount; j++) {
			//stream->writeByte(chn->stages[j].controls->blend.value.b);
			stream->writeByte((byte)(chn->stages[j].controls->gain.value.f * 255.f));
		}
	}
	// write connections
	for (var i = 0; i < mx->channelCount_; i++) {
		var chn = mx->getChannel(i);
		var devId = player_->devices().findIndex(chn->input, ModuleDevice::compareToModule);
		stream->writeByte(devId.i);
		for (var j = 0; j < chn->stageCount; j++) {
			var fxId = player_->devices().findIndex(chn->stages[j].effect, ModuleDevice::compareToModule);
			stream->writeByte(fxId.i);
		}
	}

	return (int)(stream->cursor() - start);
}
#endif
