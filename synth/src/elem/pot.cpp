#include "pot.h"
#include <math.h>
#include "base/string.h"

using namespace SYNTH;

#pragma region PotBase

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
#pragma endregion

#pragma region Pot
Pot::Pot() {
	type = PotTypeB;
}

void Pot::setFromStream(byte*& stream) {
	value.b = *stream;
	stream++;
}

S Pot::inc(int count) {
	int v = value.b + count*step.b;
	if (v > max.b) value.b = max.b;
	else value.b = v;
	return value;
}

S Pot::dec(int count) {
	int v = value.b - count * step.b;
	if (v < min.b) value.b = min.b;
	else value.b = v;
	return value;
}

float Pot::getNormalized() {
	return (float)(value.b - min.b) / (max.b - min.b);
}

S Pot::setFromNormalized(float v) {
	value.b = (int)floor(v * (max.b - min.b)) + min.b;
	if (value.b > max.b) value.b = max.b;
	else if (value.b < min.b) value.b = min.b;
	return value;
}

void Pot::getValueAsString(char* str, int len) {
	str_format_s(str, len, "%d", value.b);
}

#pragma endregion

#pragma region PotF
PotF::PotF() {
	type = PotTypeF;
}

void PotF::setFromStream(byte*& stream) {
	var f = *((float*)stream);
	value.f = f;
	stream += sizeof(float);
}

S PotF::inc(int count) {
	value.f += count*step.f;
	if (value.f > max.f) value.f = max.f;
	return value;
}

S PotF::dec(int count) {
	value.f -= count*step.f;
	if (value.f < min.f) value.f = min.f;
	return value;
}

float PotF::getNormalized() {
	return (value.f - min.f) / (max.f - min.f);
}

S PotF::setFromNormalized(float v) {
	value.f = v * (max.f - min.f) + min.f;
	if (value.f > max.f) value.f = max.f;
	else if (value.f < min.f) value.f = min.f;
	return value;
}

void PotF::getValueAsString(char* str, int len) {
	str_format_s(str, len, "%.2f", value.f);
}

#pragma endregion

#pragma region PotF8
PotF8::PotF8() {
	type = PotTypeF8;
}

void PotF8::init(S mn, S mx, S st, S val) {
	min = mn.b / 255.0f;
	max = mx.b / 255.0f;
	step = st.b / 255.0f;
	value = val.b / 255.0f;
}

void PotF8::setFromStream(byte*& stream) {
	var f = *stream / 255.0f;
	value.f = f;
	stream++;
}

float PotF8::getNormalized() {
	return value.f;
}

S PotF8::setFromNormalized(float v) {
	if (v < 0.0f) v = 0.0f;
	else if (v > 1.0f) v = 1.0f;
	value.f = v;
	return value;
}
#pragma endregion