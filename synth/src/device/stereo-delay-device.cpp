#include "base/memory.h"
#include "stereo-delay-device.h"

using namespace SYNTH;

StereoDelayDevice::StereoDelayDevice(SynthAdapter* adapter) : ModuleDevice(NEW_(StereoDelay), adapter) {
	type(DeviceStereoDelay);
}

StereoDelayDevice::~StereoDelayDevice() {
	DEL_((StereoDelay*)object_);
}

#ifdef PLAYER_EDIT_MODE
int StereoDelayDevice::writeToStream(Stream* stream) {
	var start = stream->cursor();
	Device::writeToStream(stream);
	var dly = (StereoDelay*)object_;
	stream->writeByte((byte)(dly->controls.feedbackLeft.value.f * 255.0f));
	stream->writeFloat(dly->controls.delayLeft.value.f);
	stream->writeByte((byte)(dly->controls.feedbackRight.value.f * 255.0f));
	stream->writeFloat(dly->controls.delayRight.value.f);
	stream->writeByte((byte)(dly->controls.mixLeft.value.f * 255.0f));
	stream->writeByte((byte)(dly->controls.mixRight.value.f * 255.0f));
	return (int)(stream->cursor() - start);
}
#endif