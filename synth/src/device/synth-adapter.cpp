#include <windows.h>
#include "base/memory.h"
#include "synth-adapter.h"
#include "player/src/player.h"
#include "soundlib/src/soundplayer.h"
#include "synth-device.h"
#include "mixer-device.h"
#include "distort-device.h"
#include "stereo-delay-device.h"

NS_FW_BASE_USE
using namespace SYNTH;
using namespace PLAYER;

AdapterInfo SynthAdapter::adapterInfo_ = { "SynthAdapter", 2 };

AdapterInfo* const SynthAdapter::getInfo() {
	return &SynthAdapter::adapterInfo_;
}
SynthAdapter::SynthAdapter() {
	samplingRate = 48000;
}
SynthAdapter::~SynthAdapter() {

}
void SynthAdapter::initialize(byte** pData) {
	if (pData != NULL && *pData != NULL) {
		samplingRate = (float)READ(*pData, word);
		// default bpm = 80
		//setRefreshRate(80.0f);
	}
	//SoundPlayer::start(samplingRate_, 2, SynthAdapter::fillSoundBuffer, this);
}
Device* SynthAdapter::createDevice(int deviceType) {
	Device* device = NULL;
	switch (deviceType) {
	case SynthDevices::DeviceSynth:
		device = NEW_(SynthDevice, this);
		break;
	case SynthDevices::DeviceMixer:
		device = NEW_(MixerDevice, this);
		break;
	case SynthDevices::DeviceClip:
		device = NEW_(DistortDevice, this);
		break;
	case SynthDevices::DeviceDelay:
		device = NEW_(StereoDelayDevice, this);
		break;
	}
	return device;
}

#ifdef PLAYER_EDIT_MODE
void SynthAdapter::writeToStream(Stream* stream) {
	Adapter::writeToStream(stream);
	stream->writeWord((int)samplingRate);
}
#endif
