#include "soundlib/src/soundplayer.h"
#include "base/memory.h"
#include "synth-device-type.h"
#include "synth-device.h"

using namespace SYNTH;

const int SynthDeviceType::TypeId = 2;
char* const SynthDeviceType::TypeName = "Synth";

SynthDeviceType::SynthDeviceType() : DeviceType(SynthDeviceType::TypeId, SynthDeviceType::TypeName) {
	samplingRate_ = 48000;
}
SynthDeviceType::~SynthDeviceType() {
	SoundPlayer::stop();
}

void SynthDeviceType::prepareContext(byte** pData) {
	samplingRate_ = READ(*pData, int);
	SoundPlayer::start(samplingRate_, 1, SynthDeviceType::soundCallback, parent_);
}

Device* SynthDeviceType::createDeviceImpl(int type) {
	Device* device = NULL;
	switch (type) {
	case DeviceSynth:
		device = NEW_(SynthDevice);
		break;
	}
	return device;
}

int SynthDeviceType::writeInitDataToStream(Stream* stream) {
	stream->writeDword(samplingRate_);
	return sizeof(samplingRate_);
}



void SynthDeviceType::soundCallback(short* buffer, int sampleCount, void* context) {
	var sdt = (SynthDeviceType*)context;
	for (var i = 0; i < sdt->instances().length(); i++) {
	    var dev = (Device*)sdt->instances().getAt(i);
	    dev->run(buffer, 0, sampleCount);
	}

	
}