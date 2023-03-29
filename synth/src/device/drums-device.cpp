#include "base/memory.h"
#include "player/src/player.h"
#include "synth/src/device/drums-device.h"
#include "synth/src/device/synth-base-device.h"

using namespace SYNTH;

#pragma region Creation
DrumsDevice::DrumsDevice(SynthAdapter* adapter) : ModuleDevice(NEW_(Drums), adapter) {
	type(SynthDevices::DeviceDrums);
}

DrumsDevice::~DrumsDevice() {
	if (object_ != NULL) {
		DEL_((Drums*)object_);
		object_ = NULL;
	}
}

//void DrumsDevice::initialize(byte** pData) {
//	if (pData != NULL && *pData != NULL) {
//		ModuleDevice::initialize(pData);
//		//drums()->setSoundbank(drums()->getDefaultSoundbank());
//		//var dbId = READ(*pData, byte);
//		//var db = ((DataBlockItem*)player_->dataBlocks().get(dbId));
//		//if (db != NULL) {
//		//	var sb = drums()->createSoundbank();
//		//	sb->data(db->dataBlock);
//		//	drums()->setSoundbank(sb);
//		//}
//	}
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