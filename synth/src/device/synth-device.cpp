#include "soundlib/src/soundplayer.h"
#include "base/memory.h"
#include "player/src/player.h"
#include "synth/src/device/synth-device.h"
#include "synth/src/module/synth.h"

using namespace SYNTH;

SynthDevice::SynthDevice(SynthAdapter* adapter) : SynthBaseDevice(adapter, NEW_(Synth, 3)) {
	type(SynthDevices::DeviceSynth);
}

void SynthDevice::initialize(int voiceCount) {
	object_ = NEW_(Synth, voiceCount);
}

SynthDevice::~SynthDevice() {
	if (object_ != NULL) {
		DEL_((Synth*)object_);
		object_ = NULL;
	}
}

SynthBase* SynthDevice::synth() {
	return (SynthBase*)object_;
}

Sequence* SynthDevice::createDefaultSequence() {
	var seq = NEW_(Sequence, this);
	seq->writeHeader();

	seq->writeDelta(0);
	seq->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(0x50);
	seq->writeCommand(CmdSetNote)->writeByte(pDs3)->writeByte(0x80);
	seq->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(0x80);
	seq->writeEOF();


	seq->writeDelta(16);
	seq->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(0x00);
	seq->writeCommand(CmdSetNote)->writeByte(pDs3)->writeByte(0x00);
	seq->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(0x00);
	seq->writeEOF();

	seq->writeDelta(48);
	seq->writeEOS();

	return seq;
}
