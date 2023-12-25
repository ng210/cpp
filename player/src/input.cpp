#include <math.h>
#include "player/src/input.h"
#include "base/string.h"
#include "base/memory.h"

using namespace PLAYER;

#pragma region InputBase

int InputBase::setter(void* input, Value value, void* unused) {
	((InputBase*)input)->value = value;
	return 1;
}

InputBase::InputBase() {
	set.add(this, &InputBase::setter);
	init(Value(0.0f), Value(1.0f), Value(0.01f), Value(0.0f));
}
void InputBase::init(Value min, Value max, Value step, Value value) {
	this->min = min;
	this->max = max;
	this->step = step;
	set(value);
}

#pragma endregion

#pragma region Input
Input::Input() {
	type = InputTypeB;
}

void Input::inc(int count) {
	var v = value.b + count * step.b;
	if (v > max.b) v = max.b;
	set(v);
}
void Input::dec(int count) {
	var v = value.b - count * step.b;
	if (v < min.b) v = min.b;
	set(v);
}
int Input::size() {
	return sizeof(Value::b);
}

float Input::getNormalized() {
	return (float)(value.b - min.b) / (max.b - min.b);
}
void Input::getValueAsString(char* str, int len) {
	str_format_s(str, len, "%d", value.b);
}

void Input::setFromNormalized(float f) {
	var v = (int)floor(f * (max.b - min.b)) + min.b;
	if (v > max.b) v = max.b;
	else if (v < min.b) v = min.b;
	set(v);
}
void Input::setFromStream(byte*& stream) {
	set(*stream++);
}

void Input::writeToStream(byte*& stream) {
	WRITE(stream, value.b, byte);
}
#pragma endregion

#pragma region InputF
InputF::InputF() {
	type = InputTypeF;
}

void InputF::inc(int count) {
	var v = value.f + count * step.f;
	if (v > max.f) v = max.f;
	set(v);
}
void InputF::dec(int count) {
	var v = value.f - count * step.f;
	if (v < min.f) v = min.f;
	set(v);
}
int InputF::size() {
	return sizeof(Value::f);
}

float InputF::getNormalized() {
	return (value.f - min.f) / (max.f - min.f);
}
void InputF::getValueAsString(char* str, int len) {
	str_format_s(str, len, "%.2f", value.f);
}
void InputF::setFromNormalized(float f) {
	var v = f * (max.f - min.f) + min.f;
	if (f > max.f) v = max.f;
	else if (f < min.f) v = min.f;
	set(v);
}
void InputF::setFromStream(byte*& stream) {
	set(*(float*)stream);
	stream += sizeof(float);
}
void InputF::writeToStream(byte*& stream) {
	WRITE(stream, value.f, float);
}
#pragma endregion

#pragma region InputF8
InputF8::InputF8() {
	type = InputTypeF8;
	//set.fn = &InputBase::setterF8;
}

int InputF8::size() {
	return sizeof(Value::b);
}

void InputF8::init(Value min, Value max, Value step, Value value) {
	this->min = min.b / 255.0f;
	this->max = max.b / 255.0f;
	this->step = step.b / 255.0f;
	set(value.b / 255.0f);
}

void InputF8::getValueAsString(char* str, int len) {
	str_format_s(str, len, "%d", (byte)trunc(value.f * 255.0f));
}
void InputF8::setFromStream(byte*& stream) {
	set(*stream / 255.0f);
	stream++;
}
void InputF8::writeToStream(byte*& stream) {
	WRITE(stream, (byte)trunc(value.f * 255.0f), byte);
}

#pragma endregion
