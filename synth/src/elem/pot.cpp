#include "pot.h"

using namespace SYNTH;

PotBase::PotBase() {
	init(S(0.0f), S(1.0f), S(0.01f), S(0.0f));
}

void PotBase::init(S min, S max, S step, S value) {
	this->min = min;
	this->max = max;
	this->step = step;
	this->value = value;
}

void PotBase::setFromStream(byte*& stream) {
	// placeholder
}


void Pot::setFromStream(byte*& stream) {
	value.b = *stream;
	stream++;
}

void PotF8::init(S min, S max, S step, S value) {
	min = min.b / 255.0f;
	max = max.b / 255.0f;
	step = step.b / 255.0f;
	value = value.b / 255.0f;
}

void PotF8::setFromStream(byte*& stream) {
	var f = *stream / 255.0f;
	value.f = f;
	stream++;
}

void PotF::setFromStream(byte*& stream) {
	var f = *((float*)stream);
	value.f = f;
	stream += sizeof(float);
}
