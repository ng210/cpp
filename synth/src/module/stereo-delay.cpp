#include "base/memory.h"
#include "stereo-delay.h"
#include <math.h>

NS_FW_BASE_USE
using namespace SYNTH;

StereoDelay::StereoDelay(float* samplingRate) {
	pControls_ = (PotBase*)&controls_;
	left_.samplingRate(samplingRate);
	left_.assignControls(&controls_.feedbackLeft);
	right_.samplingRate(samplingRate);
	right_.assignControls(&controls_.feedbackRight);
	createOutputBuffers(2);
	isMono_ = false;
}

StereoDelay::~StereoDelay() {
	
}

void StereoDelay::initialize(byte** pData) {
	if (pData != NULL && *pData != NULL) {
		controls_.feedbackLeft.setFromStream(*pData);
		controls_.delayLeft.setFromStream(*pData);
		controls_.feedbackRight.setFromStream(*pData);
		controls_.delayRight.setFromStream(*pData);
		controls_.mixLeft.setFromStream(*pData);
		controls_.mixRight.setFromStream(*pData);
		setDelayLeft(controls_.delayLeft.value.f);
		setDelayRight(controls_.delayRight.value.f);
	}
}

void StereoDelay::connectInput(int id, float* buffer) {
	Module::connectInput(id, buffer);
	if (id == 0) {
		left_.connect(0, buffer);
	} else {
		right_.connect(1, buffer);
	}
}

void StereoDelay::run(int start, int end) {
	for (var i = start; i < end; i++) {
		outputs_[0][i] = left_.run(inputs_[0][i]) * controls_.mixLeft.value.f;
		outputs_[1][i] = right_.run(inputs_[1][i]) * controls_.mixRight.value.f;
	}
}

void StereoDelay::setDelayLeft(float delay) {
	left_.setDelay(controls_.delayLeft.value.f);
}

void StereoDelay::setDelayRight(float delay) {
	right_.setDelay(controls_.delayRight.value.f);
}
