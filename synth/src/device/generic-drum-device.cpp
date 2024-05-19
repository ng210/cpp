#include "base/memory.h"
#include "synth/src/device/generic-drum-device.h"

using namespace SYNTH;

#pragma region Creation
GenericDrumDevice::GenericDrumDevice(SynthAdapter* adapter, Player* player) : GenericDrumDevice(adapter, player, NEW_(GenericDrum)) {
}

GenericDrumDevice::GenericDrumDevice(SynthAdapter* adapter, Player* player, GenericDrum* drumObj) : ModuleDevice(adapter, player, drumObj) {
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
	assignInputs();
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
	3,											// 2 presets in the bank

	'B', 'D', '8', '0', '8', '.', '.', '.',
	'.', '.', '.', '.', '.', '.', '.', 0,	// 1st name
	'S', 'D', '8', '0', '8', '.', '.', '.',
	'.', '.', '.', '.', '.', '.', '.', 0,	// 2nd name
	'H', 'H', '8', '0', '8', '.', '.', '.',
	'.', '.', '.', '.', '.', '.', '.', 0,	// 3rd name

	DB(DrumTypes::BassDrumType),											// type
	DF(0.9f), 1, 2, 4, 80,													// dahr1-am: amp, del, atk, hld, rel
	DF(0.1f), 1, 1, 1, 4,													// dahr2-fm: amp, del, atk, hld, rel
	DF(0.2f), 0, 0, 1, 1,													// dahr3-cl: amp, del, atk, hld, rel
	DF(0.4f), 0, 0, 0, 2,													// dahr4-cm: amp, del, atk, hld, rel
	60, 0, DB(FltMode::FmHighPass),											// flt1: cut, res, mode
	10, 80, DB(FltMode::FmLowPass),											// flt2: cut, res, mode
	160, 120, 240, 140, 0, 0,												// amp[6]
	DF(31.1f), DF(31.7f), DF(57.0f), DF(4371.12f),	DF(0.0f), DF(0.0f),		// fre[6]
	20, 0, 0, 0, 0,															// length, tune, tone, click, snap

	DB(DrumTypes::BassDrumType),											// type
	DF(0.9f), 2, 1, 3, 28,													// dahr1-am: amp, del, atk, hld, rel
	DF(0.4f), 1, 1, 1, 8,													// dahr2-fm: amp, del, atk, hld, rel
	DF(0.8f), 0, 1, 0, 4,													// dahr3-cl: amp, del, atk, hld, rel
	DF(0.4f), 1, 1, 2, 8,													// dahr4-cm: amp, del, atk, hld, rel
	120, 140, DB(FltMode::FmBandPass | FltMode::FmHighPass),				// flt1: cut, res, mode
	50, 80, DB(FltMode::FmLowPass),											// flt2: cut, res, mode
	140, 120, 100, 180, 0, 0,												// amp[6]
	DF(116.7f), DF(176.2f), DF(63.5f), DF(6371.12f), DF(0.0f), DF(0.0f),	// fre[6]
	30, 0, 0, 0, 0,															// length, tune, tone, click, snap

	DB(DrumTypes::HihatType),												// type
	DF(0.8f), 0, 1, 2, 22,													// dahr1-am: amp, del, atk, hld, rel
	DF(1.0f), 1, 1, 1, 1,													// dahr2-fm: amp, del, atk, hld, rel
	DF(0.9f), 0, 0, 1, 2,													// dahr3-cl: amp, del, atk, hld, rel
	DF(0.6f), 1, 1, 2, 8,													// dahr4-cm: amp, del, atk, hld, rel
	160, 0, DB(FltMode::FmHighPass),										// flt1: cut, res, mode
	140, 0, DB(FltMode::FmBandPass | FltMode::FmLowPass),					// flt2: cut, res, mode
	33, 35, 40, 42, 42, 37,													// amp[6]
	DF(205.2f), DF(284.0f), DF(303.0f), DF(369.2f), DF(426.0f), DF(521.0f),	// fre[6]
	120, 0, 0, 0, 0															// length, tune, tone, click, snap
};

PresetBank* GenericDrumDevice::getDefaultPresetBank() {
	return NEW_(PresetBank, getPresetBankSize(), _defaultGenericDrumPresetBank);
}

int GenericDrumDevice::setLength(void* context, Value value, void* args) {
	var dev = (GenericDrumDevice*)context;
	var values = (GenericDrumValues*)dev->drum()->getValues();
	var len = dev->length_.f * 3.9f + 0.1f;
	values->dahr[0].rel = (int)(dev->dahrRel_[0].i * len);
	values->dahr[1].rel = (int)(dev->dahrRel_[1].i * len);
	values->dahr[3].rel = (int)(dev->dahrRel_[3].i * len);
	return 1;
}

int GenericDrumDevice::setTune(void* context, Value value, void* args) {
	var dev = (GenericDrumDevice*)context;
	var values = (GenericDrumValues*)dev->drum()->getValues();
	for (var oi = 0; oi < 6; oi++) {
		values->osc[oi].fre = dev->fre_[oi];
	}
	return 1;
}

int GenericDrumDevice::setTone(void* context, Value value, void* args) {
	var dev = (GenericDrumDevice*)context;

	return 1;
}

int GenericDrumDevice::setClick(void* context, Value value, void* args) {
	var dev = (GenericDrumDevice*)context;

	return 1;
}

int GenericDrumDevice::setSnap(void* context, Value value, void* args) {
	var dev = (GenericDrumDevice*)context;

	return 1;
}

void GenericDrumDevice::assignInputs() {
	var values = (GenericDrumValues*)module()->getValues();
	var inputs = (GenericDrumInputs*)inputs_;

	//var count = offsetof(GenericDrumValues, osc) / sizeof(Value);
	//for (var ii = 0; ii < count; ii++) {
	//	inputs_[ii].setValue(&((Value*)values)[ii]);
	//}
	//for (var oi = 0; oi < 6; oi++) {
	//	inputs->fre[oi].setValue(&values->osc[oi].fre);
	//	inputs->amp[oi].setValue(&values->osc[oi].amp);
	//}

	for (var ii = 0; ii < inputCount_; ii++) {
		var input = &inputs_[ii];
		Value* pValue = NULL;
		switch (ii) {
			case GenDrumDahrRel1: pValue = &dahrRel_[0]; inputs->dahr[0].rel.set.add(this, &GenericDrumDevice::setLength); break;
			case GenDrumDahrRel2: pValue = &dahrRel_[1]; inputs->dahr[1].rel.set.add(this, &GenericDrumDevice::setLength); break;
			//case GenDrumDahrRel3: pValue = &dahrRel_[2]; inputs->dahr[0].rel.set.add(this, &GenericDrumDevice::setLength); break;
			case GenDrumDahrRel4: pValue = &dahrRel_[3]; inputs->dahr[3].rel.set.add(this, &GenericDrumDevice::setLength); break;

			case GenDrumFre1: pValue = &fre_[0]; inputs->fre[0].set.add(this, &GenericDrumDevice::setTune); break;
			case GenDrumFre2: pValue = &fre_[1]; inputs->fre[1].set.add(this, &GenericDrumDevice::setTune); break;
			case GenDrumFre3: pValue = &fre_[2]; inputs->fre[2].set.add(this, &GenericDrumDevice::setTune); break;
			case GenDrumFre4: pValue = &fre_[3]; inputs->fre[3].set.add(this, &GenericDrumDevice::setTune); break;
			case GenDrumFre5: pValue = &fre_[4]; inputs->fre[4].set.add(this, &GenericDrumDevice::setTune); break;
			case GenDrumFre6: pValue = &fre_[5]; inputs->fre[5].set.add(this, &GenericDrumDevice::setTune); break;

			case GenDrumAmp1: pValue = &values->osc[0].amp; break;
			case GenDrumAmp2: pValue = &values->osc[1].amp; break;
			case GenDrumAmp3: pValue = &values->osc[2].amp; break;
			case GenDrumAmp4: pValue = &values->osc[3].amp; break;
			case GenDrumAmp5: pValue = &values->osc[4].amp; break;
			case GenDrumAmp6: pValue = &values->osc[5].amp; break;

			default: pValue = &((Value*)values)[ii];
		}
		input->setValue(pValue);
	}

	inputs->length.setValue(&length_);
	inputs->length.set.add(this, &GenericDrumDevice::setLength);
	inputs->tune.setValue(&length_);
	inputs->tune.set.add(this, &GenericDrumDevice::setTune);
	inputs->tone.setValue(&length_);
	//inputs->tone.set.add(this, &GenericDrumDevice::setTone);
	inputs->click.setValue(&length_);
	//inputs->click.set.add(this, &GenericDrumDevice::setClick);
	inputs->snap.setValue(&length_);
	//inputs->snap.set.add(this, &GenericDrumDevice::setSnap);
}

//int GenericDrumDevice::presetSetter(void* obj, int presetId, void* args) {
//	var dev = (GenericDrumDevice*)obj;
////	dev->preset_ = presetId;
////	var pb = dev->presetBank_->getPreset(presetId);
////	if (pb != NULL) {
////		var inputs = (GenericDrumInputs*)dev->inputs_;
////		var count = offsetof(GenericDrumValues, osc) / sizeof(Value);
////		for (var ii = 0; ii < count; ii++) {
////			dev->inputs_[ii].readValueFromStream(pb);
////		}
////		for (var oi = 0; oi < 6; oi++) {
////			inputs->amp[oi].readValueFromStream(pb);
////		}
////		for (var oi = 0; oi < 6; oi++) {
////			inputs->fre[oi].readValueFromStream(pb);
////		}
////	}
////	return pb == NULL;
//	var drm = dev->drum();
//	return Device::presetSetter(obj, presetId, args);
//}