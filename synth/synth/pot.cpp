#include "pot.h"

using namespace SYNTH;

PotBase::PotBase() {
	init(S(0.0f), S(1.0f), S(0.01f), S(0.0f));
}

void PotBase::init(S min, S max, S step, S value) {
	min_ = min;
	max_ = max;
	step_ = step;
	value_ = value;
}

void Pot::setFromStream(byte*& stream) {
	value_.b = *stream;
	stream++;
}

void PotF8::init(S min, S max, S step, S value) {
	min_ = min.b / 255.0f;
	max_ = max.b / 255.0f;
	step_ = step.b / 255.0f;
	value_ = value.b / 255.0f;
}

void PotF8::setFromStream(byte*& stream) {
	var f = *stream / 255.0f;
	value_.f = f;
	stream++;
}

void PotF::setFromStream(byte*& stream) {
	var f = *((float*)stream);
	value_.f = f;
	stream += sizeof(float);
}
