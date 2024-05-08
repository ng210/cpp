#include "base/memory.h"
#include "synth/src/module/stereo-delay.h"
#include <math.h>

NS_FW_BASE_USE
using namespace SYNTH;

void Delayer::initialize(DlyValues* dlyValues, FltValues* fltValues) {
	delay.values(dlyValues);
	delay.setSamplingRate();
	delay.updateDelay();
	filter.createStages(2);
	filter.values(fltValues);
	filter.update(0.0f);
}

StereoDelay::StereoDelay() : Module() {
	left_.initialize(&values_.left, &values_.flt);
	right_.initialize(&values_.right, &values_.flt);
	createOutputBuffers(2);
	isMono_ = false;
}

StereoDelay::~StereoDelay() {
	
}

void StereoDelay::connectInput(int id, float* buffer) {
	inputs_[id % 2] = buffer;
}

float* StereoDelay::getOutput(int id) {
	return outputs_[id % 2];
}

Value* StereoDelay::getValues() {
	return (Value*)&values_;
}

void StereoDelay::run(int start, int end, BufferWriteModes mode) {
	updateFilter();
	for (var i = start; i < end; i++) {
		var delayedLeft = left_.delay.run(inputs_[0][i]);
		var delayedRight = right_.delay.run(inputs_[1][i]);

		//var fb = q + q / (1.0f - f);
		//st->ai_[0] = st->ai_[0] + f * (smp - st->ai_[0] + fb * (st->ai_[0] - st->ai_[1]));
		//smp = st->ai_[1] = st->ai_[1] + f * (st->ai_[0] - st->ai_[1]);

		var left = left_.filter.run(delayedLeft);
		var right = right_.filter.run(delayedRight);

		outputs_[0][i] = values_.mixLeft.f * left + (1.0f - values_.mixLeft.f) * inputs_[0][i];
		outputs_[1][i] = values_.mixRight.f * right + (1.0f - values_.mixRight.f) * inputs_[1][i];
	}
}

void StereoDelay::updateFilter() {
	left_.filter.update(0.0f);
	right_.filter.update(0.0f);
}


