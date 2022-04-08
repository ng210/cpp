#include "base/memory.h"
#include "synth-device.h"

using namespace SYNTH;

SynthDevice::SynthDevice(int samplingRate, int voiceCount) {
	//samplingRate_ = samplingRate;
	//voiceCount_ = voiceCount;
	synth_ = NEW_(Synth, (float)samplingRate, voiceCount);
}

SynthDevice::~SynthDevice() {
	DEL_(synth_);
}

void SynthDevice::setRefreshRate(float fps) {
}

void SynthDevice::setNote(byte note, byte velocity) {
	synth_->setNote(note, velocity);
}
void SynthDevice::setControl(byte ctrlId, byte value) {
	var ctrl = synth_->getControl(ctrlId);
	ctrl->value_.b = value;
}
void SynthDevice::setControl(byte ctrlId, float value) {
	var ctrl = synth_->getControl(ctrlId);
	ctrl->value_.f = value;
}
void SynthDevice::setProgram(byte prgId) {
	synth_->setProgram(prgId);
}

byte* SynthDevice::soundBank() {
	return synth_->soundBank();
}

void SynthDevice::soundBank(byte* data) {
	synth_->soundBank(data);
}

void SynthDevice::run(short* buffer, int start, int end) {
	synth_->run(buffer, start, end);
}