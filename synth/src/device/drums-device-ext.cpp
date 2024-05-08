#include "base/memory.h"
#include "player/src/player-ext.h"
#include "synth/src/device/drums-device-ext.h"

using namespace PLAYER;
using namespace SYNTH;

DeviceExt* DrumsDeviceExt::drumsDeviceExtCreator_(Device* device) {
	DeviceExt* deviceExt = NULL;
	switch (device->type()) {
		case SynthDevices::DeviceDrums: deviceExt = NEW_(DrumsDeviceExt, device);
	}
	return deviceExt;
}

DrumsDeviceExt::DrumsDeviceExt(Device* device) : ModuleDeviceExt(device) {
}

Sequence* DrumsDeviceExt::createDefaultSequence() {
	var seq = NEW_(Sequence);
	seq->writeHeader();
	seq->writeDelta(0);
	seq->writeCommand(CmdSetNote)->writeByte(drBD)->writeByte(0xA0);
	seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(0x40)->writeEOF();
	seq->writeDelta(16);
	seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(0x60)->writeEOF();
	seq->writeDelta(16);

	seq->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte(0x80);
	seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(0x40)->writeEOF();
	seq->writeDelta(16);
	seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(0x60)->writeEOF();
	seq->writeDelta(16);

	seq->writeCommand(CmdSetNote)->writeByte(drBD)->writeByte(0xA0);
	seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(0x40)->writeEOF();
	seq->writeDelta(16);
	seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(0x40)->writeEOF();
	seq->writeDelta(8);
	seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(0x60)->writeEOF();
	seq->writeDelta(8);

	seq->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte(0x80);
	seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(0x40)->writeEOF();
	seq->writeDelta(16);
	seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(0x60)->writeEOF();
	seq->writeDelta(16);
	seq->writeEOS();
	return seq;
}

void DrumsDeviceExt::registerExtensionCreator() {
	SynthAdapter adapter;
	var key = (adapter.getInfo()->id << 8) + SynthDevices::DeviceDrums;
	PlayerExt::addDeviceExtension(key, DrumsDeviceExt::drumsDeviceExtCreator_);
}
