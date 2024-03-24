#include "base/memory.h"
#include "synth/src/elem/elem.h"
#include "synth/src/module/module.h"

using namespace SYNTH;

Module::Module() {
	memset(inputs_, 0, 8 * sizeof(float*));
	memset(outputs_, 0, 8 * sizeof(float*));
	isMono_ = true;
	isActive_ = true;
}

Module::~Module() {
	if (outputs_[0]) {
		FREE(outputs_[0]);
	}
}

void Module::connectInput(int id, float* buffer) {
	inputs_[id] = buffer;
}

float* Module::getInput(int id) {
	return inputs_[id];
}

float* Module::getOutput(int id) {
	return outputs_[id];
}

void Module::createOutputBuffers(int count) {
	var p = MALLOC(float, SAMPLE_BUFFER_SIZE * count);
	for (var i = 0; i < count; i++) {
		outputs_[i] = p;
		p += SAMPLE_BUFFER_SIZE;
	}
}

//void Module::initializeFromStream(byte** pData) {
//}

bool Module::isActive() {
	return isActive_;
}

void Module::run(int start, int end) {
}

