#include "base/memory.h"
#include "dly.h"
#include <math.h>

using namespace SYNTH;

Dly::Dly() {
	buffer_ = NULL;
	controls_ = NULL;
	samplingRate_ = NULL;
	length_ = 0.0f;
	cursor_ = 0.0f;
}

Dly::~Dly() {
	FREE(buffer_);
}

void Dly::assignControls(PotBase* controls) {
	controls_ = (DlyCtrls*)controls;
	controls_->feedback.init(0.0f, 1.0f, 0.01f, 1.0f);
	controls_->delay.init(0.0f, DELAY_MAX / 100.0f, 5.0f, 1000.0f);
}

void Dly::setFromStream(byte* stream) {
	//Elem::setFromStream(stream, (Pot*)controls_);
	controls_->feedback.setFromStream(stream);
	controls_->delay.setFromStream(stream);
}

//void Dly::connect(int id, void* input) {
//	input_ = (float*)input;
//}

float Dly::run(Arg params) {
	var ci = (int)trunc(cursor_);
	// read delayed
	var delayed = buffer_[ci];
	// store delayed*feedback + input
	buffer_[ci] = delayed * controls_->feedback.value.f + params.f;
	cursor_++;
	if (cursor_ > length_) cursor_ -= length_;
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

void Dly::samplingRate(float* smpRate) {
	samplingRate_ = smpRate;
	var count = (int)(*smpRate * DELAY_MAX / 1000.0f);
	buffer_ = MALLOC(float, count);
	memset(buffer_, 0, sizeof(float) * count);
}

void Dly::setDelay(float delay) {
	length_ = delay * *samplingRate_ / 1000.0f;
	controls_->delay.value.f = delay;
	cursor_ = length_ - 1;
}