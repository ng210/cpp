#include "base/memory.h"
#include "player/src/player-ext.h"
#include "synth/src/module/synth-def.h"
#include "synth/src/device/synth-device.h"
#include "synth/src/device/synth-device-ext.h"

using namespace PLAYER;
using namespace SYNTH;

DeviceExt* SynthDeviceExt::synthDeviceExtCreator_(Device* device) {
	DeviceExt* deviceExt = NULL;
	switch (device->type()) {
	case SynthDevices::DeviceSynth: deviceExt = NEW_(SynthDeviceExt, device);
	}
	return deviceExt;
}

SynthDeviceExt::SynthDeviceExt(Device* device) : SynthBaseDeviceExt(device) {
}

Sequence* SynthDeviceExt::createDefaultSequence() {
	var seq = NEW_(Sequence);
	seq->writeHeader();

	seq->writeDelta(0);
	seq->writeCommand(CmdSetNote)->writeByte(pC3)->writeByte(0xC0);
	seq->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(0x80);
	seq->writeCommand(CmdSetNote)->writeByte(pDs4)->writeByte(0x80);
	seq->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(0x60);
	seq->writeEOF();
	seq->writeDelta(32);
	seq->writeCommand(CmdSetNote)->writeByte(pC3)->writeByte(0x00);
	seq->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(0x00);
	seq->writeCommand(CmdSetNote)->writeByte(pDs4)->writeByte(0x00);
	seq->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(0x00);
	seq->writeEOF();
	seq->writeDelta(16);
	seq->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(0x40);
	seq->writeCommand(CmdSetNote)->writeByte(pDs3)->writeByte(0x80);
	seq->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(0x60);
	seq->writeEOF();
	seq->writeDelta(16);
	seq->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(0x00);
	seq->writeCommand(CmdSetNote)->writeByte(pDs3)->writeByte(0x00);
	seq->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(0x00);
	seq->writeEOF();
	seq->writeDelta(32);
	seq->writeCommand(CmdSetNote)->writeByte(pC3)->writeByte(0xC0);
	seq->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(0x80);
	seq->writeCommand(CmdSetNote)->writeByte(pDs4)->writeByte(0x80);
	seq->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(0x60);
	seq->writeEOF();
	seq->writeDelta(16);
	seq->writeCommand(CmdSetNote)->writeByte(pC3)->writeByte(0x00);
	seq->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(0x00);
	seq->writeCommand(CmdSetNote)->writeByte(pDs4)->writeByte(0x00);
	seq->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(0x00);
	seq->writeEOF();
	seq->writeDelta(16);
	seq->writeEOS();
	return seq;
}

void SynthDeviceExt::registerExtensionCreator() {
	SynthAdapter adapter;
	var key = (adapter.getInfo()->id << 8) + SynthDevices::DeviceSynth;
	PlayerExt::addDeviceExtension(key, SynthDeviceExt::synthDeviceExtCreator_);
}
