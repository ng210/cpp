#include "pot.h"
#include <math.h>
#include "base/string.h"
#include "base/memory.h"

using namespace SYNTH;

#pragma region PotBase

int PotBase::setter(void* pot, S value, void* unused) {
	((PotBase*)pot)->value = value;
	return 1;
}

//int PotBase::setterF8(void* pot, S value) {
//	((PotBase*)pot)->value.f = value.b / 255.0f;
//	return 1;
//}

PotBase::PotBase() {
	set.add(this, &PotBase::setter);
	init(S(0.0f), S(1.0f), S(0.01f), S(0.0f));
}
void PotBase::init(S min, S max, S step, S value) {
	this->min = min;
	this->max = max;
	this->step = step;
	set(value);
}

#pragma endregion

#pragma region Pot
Pot::Pot() {
	type = PotTypeB;
}

void Pot::inc(int count) {
	var v = value.b + count*step.b;
	if (v > max.b) v = max.b;
	set(v);
}
void Pot::dec(int count) {
	var v = value.b - count * step.b;
	if (v < min.b) v = min.b;
	set(v);
}
int Pot::size() {
	return sizeof(S::b);
}

float Pot::getNormalized() {
	return (float)(value.b - min.b) / (max.b - min.b);
}
void Pot::getValueAsString(char* str, int len) {
	str_format_s(str, len, "%d", value.b);
}

void Pot::setFromNormalized(float f) {
	var v = (int)floor(f * (max.b - min.b)) + min.b;
	if (v > max.b) v = max.b;
	else if (v < min.b) v = min.b;
	set(v);
}
void Pot::setFromStream(byte*& stream) {
	set(*stream++);
}

void Pot::writeToStream(byte*& stream) {
	WRITE(stream, value.b, byte);
}
#pragma endregion

#pragma region PotF
PotF::PotF() {
	type = PotTypeF;
}

void PotF::inc(int count) {
	var v = value.f + count*step.f;
	if (v > max.f) v = max.f;
	set(v);
}
void PotF::dec(int count) {
	var v = value.f - count*step.f;
	if (v < min.f) v = min.f;
	set(v);
}
int PotF::size() {
	return sizeof(S::f);
}

float PotF::getNormalized() {
	return (value.f - min.f) / (max.f - min.f);
}
void PotF::getValueAsString(char* str, int len) {
	str_format_s(str, len, "%.2f", value.f);
}
void PotF::setFromNormalized(float f) {
	var v = f * (max.f - min.f) + min.f;
	if (f > max.f) v = max.f;
	else if (f < min.f) v = min.f;
	set(v);
}
void PotF::setFromStream(byte*& stream) {
	set(*(float*)stream);
	stream += sizeof(float);
}
void PotF::writeToStream(byte*& stream) {
	WRITE(stream, value.f, float);
}
#pragma endregion

#pragma region PotF8
PotF8::PotF8() {
	type = PotTypeF8;
	//set.fn = &PotBase::setterF8;
}

int PotF8::size() {
	return sizeof(S::b);
}

void PotF8::init(S min, S max, S step, S value) {
	this->min = min.b / 255.0f;
	this->max = max.b / 255.0f;
	this->step = step.b / 255.0f;
	set(value.b / 255.0f);
}

void PotF8::getValueAsString(char* str, int len) {
	str_format_s(str, len, "%d", (byte)trunc(value.f * 255.0f));
}
void PotF8::setFromStream(byte*& stream) {
	set(*stream / 255.0f);
	stream++;
}
void PotF8::writeToStream(byte*& stream) {
	WRITE(stream, (byte)trunc(value.f * 255.0f), byte);
}

#pragma endregion
