#include "base/memory.h"
#include "player/src/player.h"
#include "player/src/player-ext.h"
#include "synth/src/device/mixer-device-ext.h"

using namespace PLAYER;
using namespace SYNTH;

DeviceExt* MixerDeviceExt::mixerDeviceExtCreator_(Device* device) {
	DeviceExt* deviceExt = NULL;
	switch (device->type()) {
		case SynthDevices::DeviceMixer: deviceExt = NEW_(MixerDeviceExt, device);
	}
	return deviceExt;
}

void MixerDeviceExt::registerExtensionCreator() {
	SynthAdapter adapter;
	var key = (adapter.getInfo()->id << 8) + SynthDevices::DeviceMixer;
	PlayerExt::addDeviceExtension(key, MixerDeviceExt::mixerDeviceExtCreator_);
}

MixerDeviceExt::MixerDeviceExt(Device* device) : ModuleDeviceExt(device) { }

void MixerDeviceExt::writeToStream(Stream* stream) {
	ModuleDeviceExt::writeToStream(stream);
	var dev = (MixerDevice*)device_;
	var mx = dev->mixer();
	stream->writeByte(mx->channelCount());

	for (var i = 0; i < mx->channelCount(); i++) {
		var chn = mx->getChannel(i);
		var devId = dev->player()->devices().findIndex(chn->input, ModuleDevice::compareToModule);
		stream->writeByte(devId.i);
		stream->writeByte(chn->stageCount);
		for (var j = 0; j < chn->stageCount; j++) {
			var fxId = dev->player()->devices().findIndex(chn->stages[j].effect, ModuleDevice::compareToModule);
			stream->writeByte(fxId.i);
		}
	}

	//// write channel/stage controls
	//for (var ci = 0; ci < mx->channelCount(); ci++) {
	//	MixerChannelInputs& chn = dev->mixerInputs.channels[ci];
	//	chn.amp
	//	stream->writeByte((byte)(chn->controls->gain.value.f * 255.f));
	//	stream->writeByte((byte)(chn->controls->pan.value.f * 100.0));
	//	stream->writeByte((byte)(chn->controls->amp.value.f * 255.f));
	//	stream->writeByte(chn->stageCount);
	//	for (var j = 0; j < chn->stageCount; j++) {
	//		//stream->writeByte(chn->stages[j].controls->blend.value.b);
	//		stream->writeByte((byte)(chn->stages[j].controls->gain.value.f * 255.f));
	//	}
	//}
}
