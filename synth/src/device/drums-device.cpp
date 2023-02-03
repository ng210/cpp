#include "base/memory.h"
#include "synth/src/device/drums-device.h"
#include "synth/src/device/synth-base-device.h"

using namespace SYNTH;

#pragma region Creation
DrumsDevice::DrumsDevice(SynthAdapter* adapter) : ModuleDevice(NEW_(Drums), adapter) {
	type(SynthDevices::DeviceBass);
}

DrumsDevice::~DrumsDevice() {
	if (object_ != NULL) {
		DEL_((Drums*)object_);
		object_ = NULL;
	}
}

//void DrumsDevice::initialize(byte** pData) {
//	drums()->initialize(pData);
//}

void DrumsDevice::processCommand(byte cmd, byte*& cursor) {
	switch (cmd) {
	case ModuleCommands::CmdSetNote:
		byte note, velocity;
		note = READ(cursor, byte);
		velocity = READ(cursor, byte);
		drums()->setNote(note, velocity);
		break;
	default:
		ModuleDevice::processCommand(cmd, cursor);
		break;
	}
}