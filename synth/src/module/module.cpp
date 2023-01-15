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
	// set sound bank and program
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

void Module::setProgram(int prgId) {
	var sb = soundBank_;
	program_ = prgId;
	isActive_ = true;
	var count = (int)*sb;
	if (prgId < count) {
		var offset = *(short*)(sb + 1 + 16 * prgId + 14);
		sb += offset;
		count = *sb++;
		for (var i = 0; i < count; i++) {
			var iid = *sb++;
			var pot = getControl(iid);
			pot->setFromStream(sb);
		}
	}
}