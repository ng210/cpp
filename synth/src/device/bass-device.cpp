#include "base/memory.h"
#include "synth/src/module/bass.h"
#include "synth/src/device/bass-device.h"

using namespace SYNTH;

#pragma region Creation
BassDevice::BassDevice(SynthAdapter* adapter, Player* player) : SynthBaseDevice(adapter, player, NEW_(Bass, 1)) {
	type(SynthDevices::DeviceBass);
	inputs_ = (InputBase*)&bassInputs;
	inputCount_ = sizeof(BassInputs) / sizeof(InputBase);
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
	if (datablockId_ == 0xff && presetBank_ != NULL) {
		DEL_(presetBank_);
	}
}

int BassDevice::getPresetBankSize() {
	return BassInputSize;
}

byte _defaultBassPresetBank[] = {
	DB(3),											// 3 presets in the bank

	DB('B'), DB('a'), DB('s'), DB('s'), DB('0'), DB('1'), DB(0), DB('.'),
	DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB(0),	// 1st name
	DB('L'), DB('i'), DB('g'), DB('h'), DB('t'), DB('-'), DB('w'), DB('o'),
	DB('b'), DB('b'), DB('l'), DB('e'), DB(0), DB('.'), DB('.'), DB(0),	// 2nd name
	DB('P'), DB('u'), DB('l'), DB('s'), DB('e'), DB('6'), DB('4'), DB(0),
	DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB(0),	// 3rd name

	DF(0.80f), DB(2), DB(24), DB(100), DB(20),				// AM amp(F), atk, dec, sus(F8), rel
	DF(0.40f), DB(0), DB(34), DB(120), DB(60),				// PM amp(F), atk, dec, sus(F8), rel
	DF(0.20f), DB(6), DB(22), DB(2), DB(30),				// Ft amp(F), atk, dec, sus(F8), rel
	DB(100), DF(0.00f), DB(0), DB(0), DB(110), DB(WfPulse),	// Osc1 amp(F8), fre(F), note, tune, psw(F8), wave
	DB(60), DF(1.20f), DB(0), DB(12), DB(80), DB(WfPulse),	// Osc2 amp(F8), fre(F), note, tune, psw(F8), wave
	DB(10), DB(100), DB(FmLowPass),							// Flt1 cut, res(F8), mode

	DF(0.8f), DB(12), DB(23), DB(120), DB(40),				// AM amp(F), atk, dec, sus(F8), rel
	DF(0.7f), DB(0), DB(45), DB(160), DB(120),				// PM amp(F), atk, dec, sus(F8), rel
	DF(0.14f), DB(12), DB(42), DB(132), DB(80),				// Ft amp(F), atk, dec, sus(F8), rel
	DB(100), DF(0.0f), DB(0), DB(0), DB(50), DB(WfPulse),	// Osc1 amp(F8), fre(F), note, tune, psw(F8), wave
	DB(80), DF(0.34f), DB(0), DB(0), DB(80), DB(WfPulse),	// Osc2 amp(F8), fre(F), note, tune, psw(F8), wave
	DB(2), DB(200), DB(FmLowPass),							// Flt1 cut, res(F8), mode

	DF(0.80f), DB(8), DB(64), DB(64), DB(64),				// AM amp(F), atk, dec, sus(F8), rel
	DF(0.64f), DB(0), DB(64), DB(128), DB(64),				// PM amp(F), atk, dec, sus(F8), rel
	DF(1.00f), DB(8), DB(16), DB(128), DB(64),				// Ft amp(F), atk, dec, sus(F8), rel
	DB(128), DF(0.00f), DB(0), DB(0), DB(128), DB(WfPulse),	// Osc1 amp(F8), fre(F), note, tune, psw(F8), wave
	DB(64), DF(0.16f), DB(0), DB(12), DB(64), DB(WfPulse),	// Osc2 amp(F8), fre(F), note, tune, psw(F8), wave
	DB(64), DB(64), DB(FmLowPass)							// Flt1 cut, res(F8), mode
};

PresetBank* BassDevice::getDefaultPresetBank() {
	return NEW_(PresetBank, getPresetBankSize(), _defaultBassPresetBank);
}