#include "base/memory.h"
#include "module.h"

using namespace SYNTH;

Module::Module(PotBase* controls, int count) : controls_(controls), controlCount_(count) {
	isActive_ = true;
	memset(inputs_, 0, 8 * sizeof(float*));
	memset(outputs_, 0, 8 * sizeof(float*));
	isMono_ = true;
	setSoundbank.add(this, &Module::soundbankSetter);
	setProgram.add(this, &Module::programSetter);
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

void Module::initializeFromStream(byte** pData) {
	int prgId = 0;
	if (pData != NULL && *pData != NULL) {
		prgId = READ(*pData, byte);
	}
	setProgram(prgId);
}

int Module::writeToStream(Stream* stream) {
	stream->writeByte(program_);
	return 1;
}

int Module::writeProgram(byte* pData) {
	var start = pData;
	for (var i = 0; i < controlCount_; i++) {
		controls_[i].writeToStream(pData);
	}
	return (int)(pData - start);
}

bool Module::isActive() {
	return isActive_;
}

PotBase* Module::getControl(int id) {
	return id < controlCount_ ? &controls_[id] : NULL;
}

void Module::setControl(int id, S value) {
	var ctrl = getControl(id);
	if (ctrl != NULL) {
		ctrl->set(value);
	}
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

//void Module::soundbank(byte* data) {
//	if (soundbank_ == NULL) {
//		soundbank_ = createSoundbank();
//	}
//	soundbank_->initialize(soundbank_->size(), data[0], data);
//}

Soundbank* Module::createSoundbank() {
	int size = 0;
	for (var i = 0; i < controlCount_; i++) {
		size += controls_[i].size();
	}
	return NEW_(Soundbank, size, 0, NULL);
}

int Module::soundbankSetter(void* obj, Soundbank* sb) {
	var mdl = (Module*)obj;
	mdl->soundbank_ = sb;
	if (sb != NULL) {
		if (mdl->program_ > (int)mdl->soundbank_->size()) {
			mdl->program_ = 0;
		}
		mdl->setProgram(mdl->program_);
	}
	return 1;
}
int Module::programSetter(void* obj, int ix) {
	var mdl = (Module*)obj;
	mdl->program_ = ix;
	var prg = mdl->soundbank_->getProgram(ix);
	for (var i = 0; i < mdl->controlCount_; i++) {
		mdl->getControl(i)->setFromStream(prg);
	}
	return 1;
}

Soundbank* Module::getDefaultSoundbank() {
	return NULL;
}