#include "base/memory.h"
#include "distort-device.h"
#include "../module/distort.h"

using namespace SYNTH;

DistortDevice::DistortDevice(SynthAdapter* adapter) : ModuleDevice(NEW_(Distort, &adapter->samplingRate), adapter) {
	type(DeviceClip);
}

DistortDevice::~DistortDevice() {
	DEL_((Distort*)object_);
}