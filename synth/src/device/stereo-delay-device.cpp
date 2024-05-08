#include "base/memory.h"
#include "stereo-delay-device.h"

using namespace SYNTH;

StereoDelayDevice::StereoDelayDevice(SynthAdapter* adapter, Player* player) : ModuleDevice(adapter, player, NEW_(StereoDelay)) {
	type(DeviceStereoDelay);
	inputs_ = (InputBase*)&stereoDelayInputs;
	inputCount_ = sizeof(StereoDelayInputs) / sizeof(InputBase);
	setupDly(&stereoDelayInputs.left);
	setupDly(&stereoDelayInputs.right);
	setupFlt(&stereoDelayInputs.flt);
	stereoDelayInputs.mixLeft.setup(0, 255, 1);
	stereoDelayInputs.mixRight.setup(0, 255, 1);
	assignInputs();
	var stereoDelay = (StereoDelay*)object_;
	stereoDelayInputs.left.delay.set.add(&stereoDelay->left(), &StereoDelayDevice::delaySetter);
	stereoDelayInputs.right.delay.set.add(&stereoDelay->right(), &StereoDelayDevice::delaySetter);
}

StereoDelayDevice::~StereoDelayDevice() {
	DEL_((StereoDelay*)object_);
}

int StereoDelayDevice::delaySetter(void* obj, Value value, void* unused) {
	//that->stereoDelayInputs.right.delay.set(value);
	((Dly*)obj)->updateDelay();
	return 1;
}

byte _defaultStereoDelayPresetBank[] = {
	3,	// 3 presets

	's', 'p', 'a', 't', 'i', 'a', 'l', ' ', 
	'l', 'e', 'f', 't', '.', '.', '.', 0,

	's', 'p', 'a', 't', 'i', 'a', 'l', ' ', 
	'r', 'i', 'g', 'h', 't', '.', '.', 0,

	's', 'i', 'm', 'p', 'l', 'e', ' ', 'e',
	'c', 'h', 'o', '0', '1', '.', '.', 0,

	// feedback , delay, mix - left
	10, DF(1.0f), 120,
	// feedback , delay, mix - right
	20, DF(2.0f), 200,
	// cut, res, mode
	100, 0, FmLowPass | FmBandPass,

	// feedback , delay, mix - left
	20, DF(2.0f), 200,
	// feedback , delay, mix - right
	10, DF(1.0f), 120,
	// cut, res, mode
	100, 0, FmLowPass | FmBandPass,

	// feedback , delay, mix - left
	160, DF(201.0f), 200,
	// feedback , delay, mix - right
	180, DF(227.0f), 220,
	// cut, res, mode
	100, 80, FmBandPass
};

int StereoDelayDevice::getPresetBankSize() {
	return StereoDelayInputSize;
}

PresetBank* StereoDelayDevice::getDefaultPresetBank() {
	return NEW_(PresetBank, getPresetBankSize(), _defaultStereoDelayPresetBank);
}
