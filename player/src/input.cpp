#include <math.h>
#include "player/src/input.h"
#include "base/string.h"
#include "base/memory.h"

using namespace PLAYER;

#pragma region Input
Input::Input() {}

Input::Input(Value* pValue) {
	type = InputTypeB;
	size = sizeof(Value::b);
	value = pValue;
	set.add(this, &Input::setter);
}

Input::~Input() {
}

void Input::inc(int count) {
	var v = value->b + count * step.b;
	if (v > max.b) v = max.b;
	set(v);
}

void Input::dec(int count) {
	var v = value->b - count * step.b;
	if (v < min.b) v = min.b;
	set(v);
}

void Input::initialize(Value inMin, Value inMax, Value inStep) {
	min = inMin;
	max = inMax;
	step = inStep;
	check();
}

void Input::readFromStream(byte* stream) {
	if (stream != NULL) {
		var min = READ(stream, byte);
		var max = READ(stream, byte);
		var step = READ(stream, byte);
		var value = READ(stream, byte);
		initialize(min, max, step);
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
	return ((float)(value->b - min.b)) / (max.b - min.b);
}

void Input::getValueAsString(char* str, int len) {
	str_format_s(str, len, "%d", value->b);
}

void Input::writeValueToStream(Stream* stream) {
	stream->writeByte(value->b);
}

void Input::writeToStream(Stream* stream) {
	stream->writeByte(min.b);
	stream->writeByte(max.b);
	stream->writeByte(step.b);
	stream->writeByte(value->b);
}

int Input::setter(void* obj, Value value, void* unused) {
	var input = (Input*)obj;
	var v = (byte)((value.b - input->min.b) / input->step.b) * input->step.b + input->min.b;
	if (v > input->min.b) {
		if (v < input->max.b) {
			input->value->b = v;
		} else {
			input->value->b = input->max.b;
		}
	} else {
		input->value->b = input->min.b;
	}
	return 1;
}
#pragma endregion

#pragma region InputF
InputF::InputF() {}

InputF::InputF(Value* pValue) {
	type = InputTypeF;
	size = sizeof(Value::f);
	value = pValue;
	set.add(this, &InputF::setter);
}

void InputF::readFromStream(byte* stream) {
	if (stream != NULL) {
		var min = READ(stream, float);
		var max = READ(stream, float);
		var step = READ(stream, float);
		var value = READ(stream, float);
		Input::initialize(min, max, step);
		set(value);
	}
}

void InputF::check() {
	// min and step are considered correct
	// max = min + n*step
	max.f = (float)trunc((max.f - min.f) / step.f) * step.f + min.f;
}

void InputF::setFromNormalized(float v) {
	float range = max.f - min.f;
	set(v * range + min.f);
}

void InputF::inc(int count) {
	var v = value->f + count * step.f;
	if (v > max.f) v = max.f;
	set(v);
}

void InputF::dec(int count) {
	var v = value->f - count * step.f;
	if (v < min.f) v = min.f;
	set(v);
}

float InputF::getNormalized() {
	return (value->f - min.f) / (max.f - min.f);
}

void InputF::getValueAsString(char* str, int len) {
	str_format_s(str, len, "%.4f", value->f);
}

void InputF::writeToStream(Stream* stream) {
	stream->writeFloat(min.f);
	stream->writeFloat(max.f);
	stream->writeFloat(step.f);
	stream->writeFloat(value->f);
}

void InputF::writeValueToStream(Stream* stream) {
	stream->writeFloat(value->f);
}

int InputF::setter(void* obj, Value value, void* unused) {
	var input = (InputF*)obj;
	var v = (float)trunc((value.f - input->min.f) / input->step.f) * input->step.f + input->min.f;
	if (v > input->min.f) {
		if (v < input->max.f) {
			input->value->f = v;
		}
		else {
			input->value->f = input->max.f;
		}
	}
	else {
		input->value->f = input->min.f;
	}
	return 1;
}
#pragma endregion

#pragma region InputF8
InputF8::InputF8(Value* pV) : Input(pValue) {
	type = InputTypeF8;
	size = sizeof(Value::b);
	pValue = pV;
	value = &bValue_;
	set.add(this, &InputF8::setter);
}

int InputF8::setter(void* obj, Value value, void* unused) {
	var input = (InputF8*)obj;
	Input::setter(input, value, unused);
	input->pValue->f = input->bValue_.b / 255.0f;
	return 1;
}
#pragma endregion