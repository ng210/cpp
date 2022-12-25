#include "base/memory.h"
#include "module.h"

using namespace SYNTH;

Module::Module() {
	memset(inputs_, 0, 8 * sizeof(float*));
	memset(outputs_, 0, 8 * sizeof(float*));
}

Module::~Module() {
	if (outputs_[0]) {
		FREE(outputs_[0]);
	}
}

float* Module::getInput(int id) {
	return inputs_[id];
}

float* Module::getOutput(int id) {
	return outputs_[id];
}

void Module::initialize(byte** pData) {
}

bool Module::isActive() {
	return isActive_;
}

PotBase* Module::getControl(int id) {
	return &((PotBase*)pControls_)[id];
}

void Module::connectInput(int id, float* buffer) {
	inputs_[id] = buffer;
}

void Module::run(int start, int end) {
}

void Module::createOutputBuffers(int count) {
	var p = MALLOC(float, SAMPLE_BUFFER_SIZE * count);
	for (var i = 0; i < count; i++) {
		outputs_[i] = p;
		p += SAMPLE_BUFFER_SIZE;
	}
}