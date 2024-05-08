#include <math.h>
#include "base/memory.h"
#include "synth/src/device/distort-device.h"
#include "synth/src/module/distort.h"

using namespace SYNTH;

DistortDevice::DistortDevice(SynthAdapter* adapter, Player* player) : ModuleDevice(adapter, player, NEW_(Distort)) {
	type(DeviceDistort);
	inputs_ = (InputBase*)&distortInputs;
	inputCount_ = sizeof(DistortInputs) / sizeof(InputBase);
	setupClp(&distortInputs.clp);
	setupFlt(&distortInputs.flt);
	assignInputs();
}

DistortDevice::~DistortDevice() {
	DEL_((Distort*)object_);
}

byte _defaultDistortPresetBank[] = {
	2,	// 2 presets

	'B', 'a', 's', 's', 'B', 'o', 'o', 's',
	't', '.', '.', '.', '.', '.', '.', 0,		// 1st name
	'G', 'r', 'u', 'n', 'g', 'e', '.', '.',
	'.', '.', '.', '.', '.', '.', '.', 0,		// 2nd name

	// amp,	  lvl, cut, res, mode
	DF(2.0f), 240,  40,  80, FmLowPass,
	DF(6.0f), 100, 100, 180, FmBandPass
};

int DistortDevice::getPresetBankSize() {
	return DistortInputSize;
}

PresetBank* DistortDevice::getDefaultPresetBank() {
	return NEW_(PresetBank, getPresetBankSize(), _defaultDistortPresetBank);
}