#include "base/memory.h"
#include "dly.h"
#include <math.h>

using namespace SYNTH;

Dly::Dly() {
	buffer_ = NULL;
	length_ = 0.0f;
	cursor_ = 0.0f;
}

Dly::~Dly() {
	FREE(buffer_);
}

float Dly::run(Arg params) {
	var ci = (int)trunc(cursor_);
	cursor_++;
	if (cursor_ > length_) cursor_ -= length_;
	var cj = (int)trunc(cursor_);
	// read delayed
	var delayed = buffer_[ci];
	// store delayed*feedback + input
	buffer_[ci] = delayed * values_->feedback.f + params.f;
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

void Dly::updateDelay() {
	var delay = values_->delay.f;
	if (delay < 1.0f) delay = 1.0f;
	length_ = delay * *Elem::samplingRate / 1000.0f;
	cursor_ = length_ - 1;
}
