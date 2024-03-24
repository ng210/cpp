#include "base/memory.h"
#include "synth/src/device/generic-drum-device.h"

using namespace SYNTH;

#pragma region Creation
GenericDrumDevice::GenericDrumDevice(SynthAdapter* adapter, Player* player) : ModuleDevice(adapter, player, NEW_(GenericDrum)) {
	type(SynthDevices::DeviceGenericDrum);
	inputs_ = (InputBase*)&drumInputs;
	inputCount_ = sizeof(GenericDrumInputs) / sizeof(InputBase);
	drumInputs.type.setup(0, DrumTypes::ClapType, 1);
	for (var i=0; i<4; i++) setupDahr(&drumInputs.dahr[i]);
	// fm env
	drumInputs.dahr[1].amp.setup(0.0f, 1000.0f, 0.01f);
	setupFlt(&drumInputs.flt[0]);
	setupFlt(&drumInputs.flt[1]);
	for (var i = 0; i < 6; i++) {
		drumInputs.amp[i].setup(0, 255, 1);
		drumInputs.fre[i].setup(0.0f, 12000.0f, 0.1f);
	}
	setPreset.clear();
	setPreset.add(this, GenericDrumDevice::presetSetter);
	drumInputs.type.set.add(drum(), GenericDrum::typeSetter);
}

GenericDrumDevice::~GenericDrumDevice() {
	if (object_ != NULL) {
		DEL_((GenericDrum*)object_);
		object_ = NULL;
	}
}

void GenericDrumDevice::initialize(byte** pData) {
	if (pData != NULL && *pData != NULL) {
		ModuleDevice::initialize(pData);
		//drums()->setSoundbank(drums()->getDefaultSoundbank());
		//var dbId = READ(*pData, byte);
		//var db = ((DataBlockItem*)player_->dataBlocks().get(dbId));
		//if (db != NULL) {
		//	var sb = drums()->createSoundbank();
		//	sb->data(db->dataBlock);
		//	drums()->setSoundbank(sb);
		//}
	}
}

void GenericDrumDevice::processCommand(byte cmd, byte*& cursor) {
	switch (cmd) {
	case ModuleCommands::CmdSetNote:
		byte note, velocity;
		note = READ(cursor, byte);
		velocity = READ(cursor, byte);
		drum()->setNote(note, velocity);
		break;
	default:
		ModuleDevice::processCommand(cmd, cursor);
		break;
	}
}

int GenericDrumDevice::getPresetBankSize() {
	return GenericDrumInputSize;
}

byte _defaultGenericDrumPresetBank[] = {
	DB(3),											// 2 presets in the bank

	DB('B'), DB('D'), DB('8'), DB('0'), DB('8'), DB('.'), DB('.'), DB('.'),
	DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB(0),	// 1st name
	DB('S'), DB('D'), DB('8'), DB('0'), DB('8'), DB('.'), DB('.'), DB('.'),
	DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB(0),	// 2nd name
	DB('H'), DB('H'), DB('8'), DB('0'), DB('8'), DB('.'), DB('.'), DB('.'),
	DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB(0),	// 3rd name

	DB(DrumTypes::BassDrumType),											// type
	DF(0.9f), DB(1), DB(2), DB(2), DB(50),									// dahr1-am: amp, del, atk, hld, rel
	DF(0.2f), DB(2), DB(2), DB(2), DB(66),									// dahr2-fm: amp, del, atk, hld, rel
	DF(0.1f), DB(0), DB(0), DB(1), DB(1),									// dahr3-cl: amp, del, atk, hld, rel
	DF(0.2f), DB(0), DB(0), DB(0), DB(2),									// dahr4-cm: amp, del, atk, hld, rel
	DB(40), DB(0), DB(FltMode::FmHighPass),									// flt1: cut, res, mode
	DB(10), DB(80), DB(FltMode::FmLowPass),									// flt2: cut, res, mode
	DB(120), DB(120), DB(240), DB(40), DB(0), DB(0),						// amp[6]
	DF(18.1f), DF(17.7f), DF(57.0f), DF(4371.12f),	DF(0.0f), DF(0.0f),		// fre[6]

	DB(DrumTypes::BassDrumType),											// type
	DF(0.9f), DB(2), DB(1), DB(2), DB(22),									// dahr1-am: amp, del, atk, hld, rel
	DF(0.2f), DB(1), DB(1), DB(1), DB(4),									// dahr2-fm: amp, del, atk, hld, rel
	DF(0.6f), DB(0), DB(1), DB(0), DB(4),									// dahr3-cl: amp, del, atk, hld, rel
	DF(0.1f), DB(1), DB(1), DB(2), DB(2),									// dahr4-cm: amp, del, atk, hld, rel
	DB(100), DB(140), DB(FltMode::FmBandPass | FltMode::FmHighPass),			// flt1: cut, res, mode
	DB(80), DB(80), DB(FltMode::FmLowPass),									// flt2: cut, res, mode
	DB(120), DB(100), DB(100), DB(120), DB(0), DB(0),						// amp[6]
	DF(116.7f), DF(176.2f), DF(63.5f), DF(6371.12f), DF(0.0f), DF(0.0f),	// fre[6]

	DB(DrumTypes::HihatType),												// type
	DF(0.9f), DB(2), DB(2), DB(4), DB(16),									// dahr1-am: amp, del, atk, hld, rel
	DF(0.2f), DB(1), DB(1), DB(1), DB(2),									// dahr2-fm: amp, del, atk, hld, rel
	DF(0.8f), DB(0), DB(0), DB(1), DB(2),									// dahr3-cl: amp, del, atk, hld, rel
	DF(0.4f), DB(1), DB(1), DB(2), DB(3),									// dahr4-cm: amp, del, atk, hld, rel
	DB(200), DB(50), DB(FltMode::FmHighPass),								// flt1: cut, res, mode
	DB(180), DB(0), DB(FltMode::FmBandPass | FltMode::FmHighPass),			// flt2: cut, res, mode
	DB(33), DB(35), DB(40), DB(42), DB(42), DB(37),							// amp[6]
	DF(205.2f), DF(284.0f), DF(303.0f), DF(369.2f), DF(426.0f), DF(521.0f)	// fre[6]
	
};

PresetBank* GenericDrumDevice::getDefaultPresetBank() {
	return NEW_(PresetBank, getPresetBankSize(), _defaultGenericDrumPresetBank);
}

void GenericDrumDevice::assignInputs() {
	var values = (GenericDrumValues*)module()->getValues();
	var inputs = (GenericDrumInputs*)inputs_;
	var count = offsetof(GenericDrumValues, osc) / sizeof(Value);
	for (var ii = 0; ii < count; ii++) {
		inputs_[ii].setValue(&((Value*)values)[ii]);
	}
	for (var oi = 0; oi < 6; oi++) {
		inputs->fre[oi].setValue(&values->osc[oi].fre);
		inputs->amp[oi].setValue(&values->osc[oi].amp);
	}
}

int GenericDrumDevice::presetSetter(void* obj, int presetId, void* args) {
	var dev = (GenericDrumDevice*)obj;
//	dev->preset_ = presetId;
//	var pb = dev->presetBank_->getPreset(presetId);
//	if (pb != NULL) {
//		var inputs = (GenericDrumInputs*)dev->inputs_;
//		var count = offsetof(GenericDrumValues, osc) / sizeof(Value);
//		for (var ii = 0; ii < count; ii++) {
//			dev->inputs_[ii].readValueFromStream(pb);
//		}
//		for (var oi = 0; oi < 6; oi++) {
//			inputs->amp[oi].readValueFromStream(pb);
//		}
//		for (var oi = 0; oi < 6; oi++) {
//			inputs->fre[oi].readValueFromStream(pb);
//		}
//	}
//	return pb == NULL;
	var drm = dev->drum();
	return Device::presetSetter(obj, presetId, args);
}