#include "base/memory.h"
#include "synth/src/module/bass.h"
#include "synth/src/device/bass-device.h"

using namespace SYNTH;

#pragma region Creation
BassDevice::BassDevice(SynthAdapter* adapter) : SynthBaseDevice(adapter, NEW_(Bass, 1)) {
	type(SynthDevices::DeviceBass);
	inputs_ = (InputBase*)&bassInputs;
	inputCount_ = (3*sizeof(AdsrInputs) + 2*sizeof(OscInputs) + sizeof(FltInputs)) / sizeof(InputBase);
	setupAdsr(&bassInputs.amAdsr);
	setupAdsr(&bassInputs.pmAdsr);
	setupAdsr(&bassInputs.ftAdsr);
	setupOsc(&bassInputs.osc1);
	setupOsc(&bassInputs.osc2);
	setupFlt(&bassInputs.flt1);
	assignInputs();
}

BassDevice::~BassDevice() {
	if (object_ != NULL) {
		DEL_((Bass*)object_);
		object_ = NULL;
	}
}

int BassDevice::getPresetBankSize() {
	return 3*AdsrInputsSize + 2*OscInputsSize + FltInputsSize;
}

byte _defaultPresetBank[] = {
	DB(1),											// 1 preset in the bank

	DB('B'), DB('a'), DB('s'), DB('s'), DB('0'), DB('1'), DB('.'), DB('.'),
	DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB(0),	// 1st name

	DF(0.8f), DB(2), DB(24), DB(100), DB(20),				// AM amp(F), atk, dec, sus(F8), rel
	DF(0.1f), DB(12), DB(35), DB(120), DB(80),				// PM amp(F), atk, dec, sus(F8), rel
	DF(0.4f), DB(2), DB(20), DB(30), DB(40),				// Ft amp(F), atk, dec, sus(F8), rel
	DB(120), DF(0.0f), DB(0), DB(0), DB(120), DB(WfPulse),	// Osc1 amp(F8), fre(F), note, tune, psw(F8), wave
	DB(80), DF(0.2f), DB(0), DB(12), DB(130), DB(WfPulse),	// Osc2 amp(F8), fre(F), note, tune, psw(F8), wave
	DB(20), DB(60), DB(FmLowPass)							// Flt1 cut, res(F8), mode
};

PresetBank* BassDevice::getDefaultPresetBank() {
	return NEW_(PresetBank, getPresetBankSize(), _defaultPresetBank);
}