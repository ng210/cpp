#include "base/memory.h"
#include "stereo-delay-device.h"

using namespace SYNTH;

StereoDelayDevice::StereoDelayDevice(SynthAdapter* adapter) : ModuleDevice(NEW_(StereoDelay, &adapter->samplingRate), adapter) {
	type(DeviceClip);
}

StereoDelayDevice::~StereoDelayDevice() {
	DEL_((StereoDelay*)object_);
}