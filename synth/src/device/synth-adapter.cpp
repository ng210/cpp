#include <windows.h>
#include "base/memory.h"
#include "player/src/player.h"
#include "soundlib/src/soundplayer.h"
#include "synth/src/device/synth-adapter.h"

#include "synth/src/device/synth-base-device.h"

//#include "synth/src/device/synth2-device.h"
#include "synth/src/device/bass-device.h"
#include "synth/src/device/synth-device.h"
#include "synth/src/device/generic-drum-device.h"
#include "synth/src/device/drums-device.h"
#include "synth/src/device/mixer-device.h"
#include "synth/src/device/distort-device.h"
#include "synth/src/device/stereo-delay-device.h"

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
}
void SynthAdapter::prepare() {
	Elem::samplingRate = &samplingRate;
	Env::initialize();
	Flt::initialize();
	//Bass::prepare();
	//Synth::prepare();
	//Synth2::prepare();
	//Drums::prepare();
	//Distort::prepare();
	//StereoDelay::prepare();
	//SoundPlayer::start(samplingRate_, 2, SynthAdapter::fillSoundBuffer, this);
}
void SynthAdapter::cleanUp() {
	//Bass::cleanUp();
	//Synth::cleanUp();
	//Synth2::cleanUp();
	//Drums::cleanUp();
	//Distort::cleanUp();
	//StereoDelay::cleanUp();
}
PArray* SynthAdapter::getDevices() {
	var types = NEW_(PArray, 8);
	//types->push(NEW_(BassDevice, this));
	//types->push(NEW_(SynthDevice, this));
	//types->push(NEW_(DrumsDevice, this));
	return types;

}
Device* SynthAdapter::createDevice(int deviceType, Player* player) {
	Device* device = NULL;
	switch (deviceType) {
	case SynthDevices::DeviceSynth:
		device = NEW_(SynthDevice, this, player);
		break;
	//case SynthDevices::DeviceSynth2:
	//	device = NEW_(Synth2Device, this);
	//	break;
	case SynthDevices::DeviceBass:
		device = NEW_(BassDevice, this, player);
		break;
	case SynthDevices::DeviceGenericDrum:
		device = NEW_(GenericDrumDevice, this, player);
		break;
	case SynthDevices::DeviceDrums:
		device = NEW_(DrumsDevice, this, player);
		break;
	case SynthDevices::DeviceMixer:
		device = NEW_(MixerDevice, this, player);
		break;
	case SynthDevices::DeviceDistort:
		device = NEW_(DistortDevice, this, player);
		break;
	case SynthDevices::DeviceStereoDelay:
		device = NEW_(StereoDelayDevice, this, player);
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
