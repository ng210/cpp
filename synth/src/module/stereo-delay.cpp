#include "base/memory.h"
#include "stereo-delay.h"
#include <math.h>

NS_FW_BASE_USE
using namespace SYNTH;

StereoDelay::StereoDelay() {
	pControls_ = (PotBase*)&controls_;
	left_.setSamplingRate();
	left_.assignControls(&controls_.feedbackLeft);
	right_.setSamplingRate();
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
		var delayedLeft = left_.run(inputs_[0][i]);
		var delayedRight = right_.run(inputs_[1][i]);

		//var fb = q + q / (1.0f - f);
		//st->ai_[0] = st->ai_[0] + f * (smp - st->ai_[0] + fb * (st->ai_[0] - st->ai_[1]));
		//smp = st->ai_[1] = st->ai_[1] + f * (st->ai_[0] - st->ai_[1]);

		outputs_[0][i] = controls_.mixLeft.value.f * delayedLeft + (1.0f - controls_.mixLeft.value.f) * inputs_[0][i];
		outputs_[1][i] = controls_.mixRight.value.f * delayedRight + (1.0f - controls_.mixRight.value.f) * inputs_[1][i];
	}
}

void StereoDelay::setDelayLeft(float delay) {
	left_.setDelay(controls_.delayLeft.value.f);
}

void StereoDelay::setDelayRight(float delay) {
	right_.setDelay(controls_.delayRight.value.f);
}
