#include "base/memory.h"
#include "player/src/player-ext.h"
#include "synth/src/module/synth-def.h"
#include "synth/src/device/bass-device.h"
#include "synth/src/device/bass-device-ext.h"

using namespace PLAYER;
using namespace SYNTH;

DeviceExt* BassDeviceExt::bassDeviceExtCreator_(Device* device) {
	DeviceExt* deviceExt = NULL;
	switch (device->type()) {
		case SynthDevices::DeviceBass: deviceExt = NEW_(BassDeviceExt, device);
	}
	return deviceExt;
}

BassDeviceExt::BassDeviceExt(Device* device) : SynthBaseDeviceExt(device) {
}

Sequence* BassDeviceExt::createDefaultSequence() {
	var seq = NEW_(Sequence);
	seq->writeHeader();
	seq->writeDelta(0);
	seq->writeCommand(CmdSetNote)->writeByte(pC1)->writeByte(0x80)->writeEOF();
	seq->writeDelta(16);
	seq->writeCommand(CmdSetNote)->writeByte(pC1)->writeByte(0x00)->writeEOF();

	seq->writeDelta(16);
	seq->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(0x80)->writeEOF();
	seq->writeDelta(16);
	seq->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(0x00)->writeEOF();

	seq->writeDelta(16);
	seq->writeCommand(CmdSetNote)->writeByte(pC1)->writeByte(0x80)->writeEOF();
	seq->writeDelta(8);
	seq->writeCommand(CmdSetNote)->writeByte(pC1)->writeByte(0x00)->writeEOF();

	seq->writeDelta(8);
	seq->writeCommand(CmdSetNote)->writeByte(pC1)->writeByte(0x80)->writeEOF();
	seq->writeDelta(8);
	seq->writeCommand(CmdSetNote)->writeByte(pC1)->writeByte(0x00)->writeEOF();

	seq->writeDelta(8);
	seq->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(0x80)->writeEOF();
	seq->writeDelta(16);
	seq->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(0x00)->writeEOF();
	seq->writeDelta(16);
	seq->writeEOS();


	//for (var i = 0; i < 8; i++) {
	//	seq->writeCommand(CmdSetNote)->writeByte((i>>1) % 2 ? pC2 : pC1)->writeByte(0x80 * (1 - i % 2));
	//	seq->writeEOF();
	//	seq->writeDelta(16);
	//}
	//seq->writeEOS();
	return seq;
}

void BassDeviceExt::registerExtensionCreator() {
	SynthAdapter adapter;
	var key = (adapter.getInfo()->id << 8) + SynthDevices::DeviceBass;
	PlayerExt::addDeviceExtension(key, BassDeviceExt::bassDeviceExtCreator_);
}
