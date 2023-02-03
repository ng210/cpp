#include <math.h>
#include "base/memory.h"
#include "synth/src/device/distort-device.h"
#include "synth/src/module/distort.h"

using namespace SYNTH;

DistortDevice::DistortDevice(SynthAdapter* adapter) : ModuleDevice(NEW_(Distort, &adapter->samplingRate), adapter) {
	type(DeviceClip);
}

DistortDevice::~DistortDevice() {
	DEL_((Distort*)object_);
}

#ifdef PLAYER_EDIT_MODE
int DistortDevice::writeToStream(Stream* stream) {
	var start = stream->cursor();
	Device::writeToStream(stream);
	// write controls
	var dist = (Distort*)object_;
	stream->writeFloat(dist->controls().amp.value.f);
	stream->writeByte((byte)(dist->controls().lvl.value.f * 255.0f));
	stream->writeByte(dist->controls().cut.value.b);
	stream->writeByte((byte)(dist->controls().res.value.f * 255.0f));
	stream->writeByte(dist->controls().mode.value.b);
	return (int)(stream->cursor() - start);
}
#endif