#include "soundlib/src/soundplayer.h"
#include "base/memory.h"
#include "player/src/player.h"
#include "synth/src/device/synth2-device.h"
#include "synth/src/module/synth2.h"

using namespace SYNTH;

#pragma region Creation
Synth2Device::Synth2Device(SynthAdapter* adapter) : SynthBaseDevice(adapter, NEW_(Synth2, 3)) {
	type(SynthDevices::DeviceSynth2);
}

void Synth2Device::initialize(int voiceCount) {
	object_ = NEW_(Synth2, voiceCount);
}

Synth2Device::~Synth2Device() {
	if (object_ != NULL) {
		DEL_((Synth2*)object_);
		object_ = NULL;
	}
}
#pragma endregion

SynthBase* Synth2Device::synth() {
	return (SynthBase*)object_;
}


Sequence* Synth2Device::createDefaultSequence() {
	var seq = NEW_(Sequence, this);
	seq->writeHeader();

	seq->writeDelta(0);
	seq->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(0x80);
	seq->writeCommand(CmdSetNote)->writeByte(pDs2)->writeByte(0x70);
	seq->writeCommand(CmdSetNote)->writeByte(pG1)->writeByte(0x60);
	seq->writeEOF();

	seq->writeDelta(16);
	seq->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(0x00);
	seq->writeCommand(CmdSetNote)->writeByte(pDs2)->writeByte(0x00);
	seq->writeCommand(CmdSetNote)->writeByte(pG1)->writeByte(0x00);
	seq->writeEOF();

	seq->writeDelta(48);
	seq->writeEOS();
	return seq;
}