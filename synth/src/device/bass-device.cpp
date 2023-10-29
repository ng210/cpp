#include "base/memory.h"
#include "synth/src/device/bass-device.h"

using namespace SYNTH;

#pragma region Creation
BassDevice::BassDevice(SynthAdapter* adapter) : SynthBaseDevice(adapter, NEW_(Bass, 1)) {
	type(SynthDevices::DeviceBass);
}

BassDevice::~BassDevice() {
	if (object_ != NULL) {
		DEL_((Bass*)object_);
		object_ = NULL;
	}
}

Sequence* BassDevice::createDefaultSequence() {
	var seq = NEW_(Sequence, this);
	seq->writeHeader();

	seq->writeDelta(0);
	seq->writeCommand(CmdSetNote)->writeByte(pC1)->writeByte(0x80);
	seq->writeEOF();

	seq->writeDelta(16);
	seq->writeCommand(CmdSetNote)->writeByte(pC1)->writeByte(0x00);
	seq->writeEOF();

	seq->writeDelta(16);
	seq->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(160);
	seq->writeEOF();

	seq->writeDelta(8);
	seq->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(0x00);
	seq->writeEOF();

	seq->writeDelta(8);
	seq->writeCommand(CmdSetNote)->writeByte(pC1)->writeByte(0x80);
	seq->writeEOF();

	seq->writeDelta(16);
	seq->writeCommand(CmdSetNote)->writeByte(pC1)->writeByte(0x00);
	seq->writeEOF();

	seq->writeDelta(32);
	seq->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(0x80);
	seq->writeEOF();

	seq->writeDelta(16);
	seq->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(0x00);
	seq->writeEOF();

	seq->writeDelta(16);
	seq->writeEOS();
	return seq;
}
