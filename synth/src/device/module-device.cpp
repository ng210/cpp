#include "base/memory.h"
#include "player/src/player.h"
#include "synth/src/device/synth-adapter.h"
#include "synth/src/device/module-device.h"
#include "synth/src/module/module.h"

using namespace SYNTH;

ModuleDevice::ModuleDevice(Adapter* adapter, void* obj) : Device(adapter, obj) {
}

void ModuleDevice::initialize(byte** pData) {
	var pb = getDefaultPresetBank();
	if (pData != NULL && *pData != NULL) {
		datablockId_ = READ(*pData, byte);
		var db = ((DataBlockItem*)player_->dataBlocks().get(datablockId_));
		if (db != NULL) {
			pb = NEW_(PresetBank, getPresetBankSize(), db->dataBlock);
		}
		setPresetBank(pb);
	}
	// set program and further initializations
	((Module*)object_)->initializeFromStream(pData);
}

bool ModuleDevice::isActive() {
	return ((Module*)object_)->isActive();
}

int ModuleDevice::run(int ticks) {
	return 0;
}

void ModuleDevice::processCommand(byte cmd, byte*& cursor) {
	byte ctrlId, prgId;
	byte b;
	float f;
	switch (cmd) {
	case ModuleCommands::CmdSetUint8:
		ctrlId = READ(cursor, byte);
		b = READ(cursor, byte);
		setInput(ctrlId, b);
		break;
	case ModuleCommands::CmdSetFloat8:
		ctrlId = READ(cursor, byte);
		b = READ(cursor, byte);
		setInput(ctrlId, b / 255.0f);
		break;
		// case psynth.SynthAdapter.Commands.SETCTRL16:
		// 	device.setControl(sequence.getUint8(cursor++), sequence.getUint16(cursor));
		// 	cursor += 2;
		// 	break;
	case ModuleCommands::CmdSetFloat:
		ctrlId = READ(cursor, byte);
		f = READ(cursor, float);
		setInput(ctrlId, f);
		break;
	case ModuleCommands::CmdSetProgram:
		prgId = READ(cursor, byte);
		setInput(0, prgId);
		break;
	}
}

int ModuleDevice::compareToModule(COLLECTION_ARGUMENTS) {
	var mdl1 = ((ModuleDevice*)value)->object();
	return (int)((size_t)mdl1 - (size_t)key.p);
}

Map* ModuleDevice::loadPresetBanks(const char* presetBankPath, SynthAdapter* synthAdapter) {
	var presetBanks = NEW_(Map, sizeof(int), MAP_USE_REF, Map::hashingInt, Collection::compareInt);
	byte* bytes = NULL, * end = NULL;
	// try to read file
	var bytesRead = File::read(presetBankPath, &bytes);
	if (bytesRead != -1) {
		var bptr = bytes;
		end = bytes + bytesRead;
		while (bptr < end) {
			// read device type
			var deviceType = READ(bptr, byte);
			// create dummy device
			var dev = (ModuleDevice*)synthAdapter->createDevice(deviceType, NULL);
			var pb = NEW_(PresetBank, dev->getPresetBankSize(), NULL);
			pb->initializeFromStream(bptr);
			presetBanks->put(deviceType, pb);
			DEL_(dev);
		}
	}
	FREE(bytes);
	return presetBanks;
}

void ModuleDevice::setupAdsr(AdsrInputs* inputs) {
	var adsr = (AdsrInputs*)inputs;
	adsr->amp.setup(0.0f, 1.0f, 0.01f);
	adsr->atk.setup(0, 255, 1);
	adsr->dec.setup(0, 255, 1);
	adsr->sus.setup(0, 255, 1);
	adsr->rel.setup(0, 255, 1);
}
void ModuleDevice::setupDahr(DahrInputs* inputs) {
	var dahr = (DahrInputs*)inputs;
	dahr->amp.setup(0.0f, 1.0f, 0.01f);
	dahr->atk.setup(0, 255, 1);
	dahr->del.setup(0, 255, 1);
	dahr->hld.setup(0, 255, 1);
	dahr->rel.setup(0, 255, 1);
	}
void ModuleDevice::setupOsc(OscInputs* inputs) {
	var osc = (OscInputs*)inputs;
	osc->amp.setup(0, 255, 1);
	osc->fre.setup(0.0f, 2000.f, 0.01f);
	osc->note.setup(0, 255, 1);
	osc->tune.setup(0, 255, 1);
	osc->psw.setup(0, 255, 1);
	osc->wave.setup(0, 255, 1);
	}
void ModuleDevice::setupLfo(LfoInputs* inputs) {
	var lfo = (LfoInputs*)inputs;
	lfo->amp.setup(0.0f, 100.0f, 0.1f);
	lfo->fre.setup(0.0f, 10.0f, 0.01f);
}
void ModuleDevice::setupFlt(FltInputs* inputs) {
	var flt = (FltInputs*)inputs;
	flt->cut.setup(0, 255, 1);
	flt->res.setup(0, 255, 1);
	flt->mode.setup(0, 7, 1);
}
void ModuleDevice::setupClp(ClpInputs* inputs) {
	var clp = (ClpInputs*)inputs;
	clp->amp.setup(0.0f, 10.0f, 0.1f);
	clp->lvl.setup(0.0f, 10.0f, 0.1f);
}
void ModuleDevice::setupDly(DlyInputs* inputs) {
	var dly = (DlyInputs*)inputs;
	dly->delay.setup(0.0f, 10.0f, 0.01f);
	dly->feedback.setup(0, 255, 1);
}

void ModuleDevice::assignInputs() {
	var values = module()->getValues();
	for (var ii = 0; ii < inputCount_; ii++) {
		inputs_[ii].setValue(&values[ii]);
	}
}