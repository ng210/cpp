#include <math.h>
#include "player/src/input-ext.h"
#include "base/memory.h"
#include "base/string.h"

using namespace PLAYER;

void InputExt::set(Input* input, Value min, Value max, Value step, Value value) {
	input->setup(min, max, step);
	input->set(value);
}

void InputExt::setFromNormalized(Input* input, float f) {
	Value v;
	switch (input->type) {
	case InputTypeB:
		v = (byte)round(f * (input->max.b - input->min.b) / input->step.b) * input->step.b + input->min.b;
		break;
	case InputTypeF:
	case InputTypeF8:
		v = (float)round(f * (input->max.f - input->min.f) / input->step.f) * input->step.f + input->min.f;
		if (v.f > input->max.f) v = input->max.f;
		else if (v.f < input->min.f) v = input->min.f;
		break;
	}
	input->set(v);
}

void InputExt::getValueAsString(Input* input, char* str, int len) {
	switch (input->type) {
	case InputTypeB:
		str_format_s(str, len, "%d", input->value()->b);
		break;
	case InputTypeF:
		str_format_s(str, len, "%.2f", input->value()->f);
		break;
	case InputTypeF8:
		str_format_s(str, len, "%d", (byte)trunc(input->value()->f * 255.0f));
		break;
	}
}

float InputExt::getNormalized(Input* input) {
	float f = 0.0f;
	switch (input->type) {
	case InputTypeB:
		f = (float)(input->value()->b - input->min.b) / (input->max.b - input->min.b);
		break;
	case InputTypeF:
	case InputTypeF8:
		f = (input->value()->b - input->min.b) / (float)(input->max.b - input->min.b);
		break;
	}
	return f;
}

int InputExt::writeToStream(Input* input, byte*& stream) {
	var start = stream;
	switch (input->type) {
	case InputTypeB:
		WRITE(stream, input->min.b, byte);
		WRITE(stream, input->max.b, byte);
		WRITE(stream, input->step.b, byte);
		WRITE(stream, input->value()->b, byte);
		break;
	case InputTypeF:
		WRITE(stream, input->min.f, float);
		WRITE(stream, input->max.f, float);
		WRITE(stream, input->step.f, float);
		WRITE(stream, input->value()->f, float);
		break;
	case InputTypeF8:
		WRITE(stream, input->min.f, float);
		WRITE(stream, input->max.f, float);
		WRITE(stream, input->step.f, float);
		WRITE(stream, (byte)trunc(InputExt::getNormalized(input) * 255.0f), byte);
		break;
	}
	return (int)(stream - start);
}

int InputExt::writeValueToStream(Input* input, byte*& stream) {
	var start = stream;
	switch (input->type) {
	case InputTypeB:
		WRITE(stream, input->value()->b, byte);
		break;
	case InputTypeF:
		WRITE(stream, input->value()->f, float);
		break;
	case InputTypeF8:
		WRITE(stream, (byte)trunc(InputExt::getNormalized(input) * 255.0f), byte);
		break;
	}
	return (int)(stream - start);
}

bool InputExt::compare(InputBase* input, int fieldId, Value value) {
	float diff = 0.0f;
	var result = false;
	Value* inputValue = NULL;
	switch (fieldId) {
		case 0: inputValue = &input->min; break;
		case 1: inputValue = &input->max; break;
		case 2: inputValue = &input->step; break;
		default: inputValue = input->type == InputTypeF8 ? input->pValue(): input->value_; break;
	}

	switch (input->type) {
		case InputTypeF:
			diff = inputValue->f - value.f;
			result = diff > -0.001 || diff < 0.001;
			break;
		default:
			result = *inputValue == value;
	}
	return result;
}