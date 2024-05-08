#include "base/memory.h"
#include "player/src/player.h"
#include "synth/src/device/drums-device.h"
#include "synth/src/device/synth-base-device.h"

using namespace SYNTH;

#pragma region Creation
DrumsDevice::DrumsDevice(SynthAdapter* adapter, Player* player) : ModuleDevice(adapter, player, NEW_(Drums)) {
	type(SynthDevices::DeviceDrums);
	inputs_ = (InputBase*)&drumsInputs;
	inputCount_ = sizeof(DrumsInputs) / sizeof(InputBase);
	for (var di = 0; di < DRUMS_COUNT; di++) {
		drumsInputs.drums[di].bankId.setup(0, 8, 1);
		drumsInputs.drums[di].prgId.setup(0, 8, 1);
		drumsInputs.drums[di].pan.setup(0, 255, 1);
		drumsInputs.drums[di].send[0].setup(0, 255, 1);
		drumsInputs.drums[di].send[1].setup(0, 255, 1);
		drumsInputs.drums[di].send[2].setup(0, 255, 1);
		drumsInputs.drums[di].send[3].setup(0, 255, 1);
		drumDevices_[di] = NEW_(GenericDrumDevice, (SynthAdapter*)adapter_, player_, &drums()->drum()[di]);
	}
	assignInputs();
	drumPresetBanks_.init(16);
	setPreset.add(this, DrumsDevice::presetSetter);
}

DrumsDevice::~DrumsDevice() {
	if (object_ != NULL) {
		DEL_((Drums*)object_);
		object_ = NULL;
	}

	for (var di = 0; di < DRUMS_COUNT; di++) {
		drumDevices_[di]->object(NULL);
		DEL_(drumDevices_[di]);
	}
}

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
	}
}

int DrumsDevice::getPresetBankSize() {
	return DrumsInputSize;
}

byte _defaultDrumsPresetBank[] = {
	1,		// 1 preset

	DB('D'), DB('e'), DB('f'), DB('a'), DB('u'), DB('l'), DB('t'), DB('.'),
	DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB(0),	// 1st name

	// bank id, prg id, pan, send1, send2, send3, send4
	0,0, 128, 0, 0, 0, 0,  0,1, 100, 0, 0, 0, 0,  0,2, 140, 0, 0, 0, 0,  0,3, 128, 0, 0, 0, 0,
	0,4, 128, 0, 0, 0, 0,  0,5, 128, 0, 0, 0, 0,  0,6, 128, 0, 0, 0, 0,  0,7, 128, 0, 0, 0, 0
};

PresetBank* DrumsDevice::getDefaultPresetBank() {
	return NEW_(PresetBank, getPresetBankSize(), _defaultDrumsPresetBank);
}

int DrumsDevice::presetSetter(void* obj, int presetId, void* args) {
	//var ret = Device::presetSetter(obj, presetId, args);
	var drums = (DrumsDevice*)obj;
	for (var di = 0; di < DRUMS_COUNT; di++) {
		var bankId = drums->drumsInputs.drums[di].bankId.value_->b;
		drums->drum()[di]->setPresetBank((PresetBank*)drums->drumPresetBanks_.get(bankId));
		var prgId = drums->drumsInputs.drums[di].prgId.value_->b;
		drums->drum()[di]->setPreset(prgId);
	}
	return 1;
}