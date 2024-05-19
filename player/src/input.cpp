#include <math.h>
#include "player/src/input.h"
#include "base/string.h"
#include "base/memory.h"

using namespace PLAYER;

#pragma region Input base
InputBase::InputBase() {}

InputBase::InputBase(Value* pValue) {
	setValue(pValue);
}

InputBase::~InputBase() {}

void InputBase::setValue(Value* pValue) {
	value_ = pValue;
}

void InputBase::setup(Value inMin, Value inMax, Value inStep) {
	min = inMin;
	max = inMax;
	step = inStep;
	check();
}
#pragma endregion

#pragma region Input
Input::Input() {
	initialize();
}

Input::Input(Value* pValue) : InputBase(pValue) {
	initialize();
}

void Input::initialize() {
	size = sizeof(Value::b);
	type = InputTypeB;
	set.add(this, &Input::setter);
}

Input::~Input() {
}

void Input::inc(int count) {
	var v = value_->b + count * step.b;
	if (v > max.b) v = max.b;
	set(v);
}

void Input::dec(int count) {
	var v = value_->b - count * step.b;
	if (v < min.b) v = min.b;
	set(v);
}

float Input::getRange() {
	return (max.i - min.i) / (float)step.i;
}

void Input::readValueFromStream(byte*& stream) {
	var value = READ(stream, byte);
	set(value);
}

void Input::readFromStream(byte*& stream) {
	if (stream != NULL) {
		var min = READ(stream, byte);
		var max = READ(stream, byte);
		var step = READ(stream, byte);
		var value = READ(stream, byte);
		setup(min, max, step);
		set(value);
	}
}

void Input::check() {
	// min and step are considered correct
	// max = min + n*step
	max.b = ((max.b - min.b) / step.b) * step.b + min.b;
}

void Input::setFromNormalized(float v) {
	int range = max.b - min.b;
	set((int)(trunc(v * range + min.b)));
}

float Input::getNormalized() {
	return ((float)(value_->b - min.b)) / (max.b - min.b);
}

void Input::getValueAsString(char* str, int len) {
	str_format_s(str, len, "%d", value_->b);
}

void Input::writeValueToStream(Stream* stream) {
	stream->writeByte(value_->b);
}

void Input::writeToStream(Stream* stream) {
	stream->writeByte(min.b);
	stream->writeByte(max.b);
	stream->writeByte(step.b);
	stream->writeByte(value_->b);
}

int Input::setter(void* obj, Value value, void* unused) {
	var input = (Input*)obj;
	var v = (byte)((value.b - input->min.b) / input->step.b) * input->step.b + input->min.b;
	if (v >= input->min.b) {
		if (v <= input->max.b) {
			input->value_->b = v;
		} else {
			input->value_->b = input->max.b;
		}
	} else {
		input->value_->b = input->min.b;
	}
	return 1;
}
#pragma endregion

#pragma region InputF
InputF::InputF() {
	initialize();
}

InputF::InputF(Value* pValue) : InputBase(pValue) {
	initialize();
}

void InputF::initialize() {
	type = InputTypeF;
	size = sizeof(Value::f);
	set.add(this, &InputF::setter);
}

void InputF::readValueFromStream(byte*& stream) {
	var value = READ(stream, float);
	set(value);
}

void InputF::readFromStream(byte*& stream) {
	if (stream != NULL) {
		var min = READ(stream, float);
		var max = READ(stream, float);
		var step = READ(stream, float);
		var value = READ(stream, float);
		InputBase::setup(min, max, step);
		set(value);
	}
}

void InputF::check() {
	// min and step are considered correct
	// max = min + n*step
	max.f = roundf((max.f - min.f) / step.f) * step.f + min.f;
}

void InputF::setFromNormalized(float v) {
	float range = max.f - min.f;
	set(v * range + min.f);
}

void InputF::inc(int count) {
	var v = value_->f + count * step.f;
	if (v > max.f) v = max.f;
	set(v);
}

void InputF::dec(int count) {
	var v = value_->f - count * step.f;
	if (v < min.f) v = min.f;
	set(v);
}

float InputF::getRange() {
	return (max.f - min.f) / step.f;
}


float InputF::getNormalized() {
	return (value_->f - min.f) / (max.f - min.f);
}

void InputF::getValueAsString(char* str, int len) {
	str_format_s(str, len, "%.2f", value_->f);
}

void InputF::writeToStream(Stream* stream) {
	stream->writeFloat(min.f);
	stream->writeFloat(max.f);
	stream->writeFloat(step.f);
	stream->writeFloat(value_->f);
}

void InputF::writeValueToStream(Stream* stream) {
	stream->writeFloat(value_->f);
}

int InputF::setter(void* obj, Value value, void* unused) {
	var input = (InputF*)obj;
	var v = roundf((value.f - input->min.f) / input->step.f) * input->step.f + input->min.f;
	if (v >= input->min.f) {
		if (v <= input->max.f) {
			input->value_->f = v;
		}
		else {
			input->value_->f = input->max.f;
		}
	}
	else {
		input->value_->f = input->min.f;
	}
	return 1;
}
#pragma endregion

#pragma region InputF8
InputF8::InputF8() {
	initialize();
}

InputF8::InputF8(Value* pV) : Input(pV) {
	initialize();
	setValue(pV);
}

void InputF8::initialize() {
	type = InputTypeF8;
	size = sizeof(Value::b);
	value_ = &bValue_;
	setup(0, 255, 1);
	set.clear();
	set.add(this, &InputF8::setter);
}

void InputF8::setValue(Value* v) {
	pValue_ = v;
}

int InputF8::setter(void* obj, Value value, void* unused) {
	var input = (InputF8*)obj;
	Input::setter(input, value, unused);
	input->pValue()->f = input->bValue_.b / 255.0f;
	return 1;
}
#pragma endregion
