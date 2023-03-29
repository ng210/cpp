#include "base/memory.h"
#include "dly.h"
#include <math.h>

using namespace SYNTH;

Dly::Dly() : Elem(DlyCtrlCount) {
	buffer_ = NULL;
	controls_ = NULL;
	length_ = 0.0f;
	cursor_ = 0.0f;
}

Dly::~Dly() {
	FREE(buffer_);
}

void Dly::assignControls(PotBase* controls) {
	controls_ = controls;
	var ctrls = (DlyCtrls*)controls_;
	ctrls->feedback.init(0, 255, 1, 40);
	ctrls->delay.init(0.0f, DELAY_MAX, 1.0f, 1000.0f);
}

void Dly::setFromStream(byte*& stream) {
	var ctrls = (DlyCtrls*)controls_;
	ctrls->feedback.setFromStream(stream);
	var delay = READ(stream, float);
	setDelay(delay);
	//ctrls->delay.setFromStream(stream);
}

//void Dly::connect(int id, void* input) {
//	input_ = (float*)input;
//}

float Dly::run(Arg params) {
	var ci = (int)trunc(cursor_);
	cursor_++;
	if (cursor_ > length_) cursor_ -= length_;
	var cj = (int)trunc(cursor_);
	var ctrls = (DlyCtrls*)controls_;
	// read delayed
	var delayed = buffer_[ci];
	// store delayed*feedback + input
	buffer_[ci] = delayed * ctrls->feedback.value.f + params.f;
	return delayed;
}

//void Dly::run(float* buffer, int start, int end) {
//	for (var i = start; i < end; i++) {
//		var ci = (int)trunc(cursor_);
//		// read delayed
//		var delayed = buffer_[ci];
//		// store delayed to output
//		buffer[i] = delayed;
//		// store delayed*feedback + input
//		buffer_[ci] = delayed * controls_->feedback.value.f + input_[i];
//		cursor_++;
//		if (cursor_ > length_) cursor_ -= length_;
//	}
//}

void Dly::setSamplingRate() {
	var count = (int)(*Elem::samplingRate * DELAY_MAX / 1000.0f);
	buffer_ = MALLOC(float, count);
	memset(buffer_, 0, sizeof(float) * count);
}

void Dly::setDelay(float delay) {
	if (delay < 1.0f) delay = 1.0f;
	var ctrls = (DlyCtrls*)controls_;
	length_ = delay * *Elem::samplingRate / 1000.0f;
	ctrls->delay.value.f = delay;
	cursor_ = length_ - 1;
}
