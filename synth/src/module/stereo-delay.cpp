#include "base/memory.h"
#include "stereo-delay.h"
#include <math.h>

NS_FW_BASE_USE
using namespace SYNTH;

Soundbank* StereoDelay::defaultSoundbank_ = NULL;

StereoDelay::StereoDelay() : Module((PotBase*)&controls, StereoDelayCtrlCount) {
	left_.setSamplingRate();
	left_.assignControls(&controls.feedbackLeft);
	right_.setSamplingRate();
	right_.assignControls(&controls.feedbackRight);
	fltL_.createStages(2);
	fltL_.assignControls(&controls.cut);
	fltR_.createStages(2);
	fltR_.assignControls(&controls.cut);
	updateFilter();
	program_ = 0;
	createOutputBuffers(2);
	isMono_ = false;
	setSoundbank(getDefaultSoundbank());

	controls_[stdlDelayLeft].set.add(this, &StereoDelay::delayLeftSetter);
	controls_[stdlDelayRight].set.add(this, &StereoDelay::delayRightSetter);
}

StereoDelay::~StereoDelay() {
	
}

//void StereoDelay::initializeFromStream(byte** pData) {
//	if (pData != NULL && *pData != NULL) {
//		left_.setFromStream(*pData);
//		right_.setFromStream(*pData);
//		controls.mixLeft.setFromStream(*pData);
//		controls.mixRight.setFromStream(*pData);
//		controls.cut.setFromStream(*pData);
//		controls.res.setFromStream(*pData);
//		controls.mode.setFromStream(*pData);
//		updateFilter();
//	}
//}

void StereoDelay::connectInput(int id, float* buffer) {
	Module::connectInput(id, buffer);
	if (id == 0) {
		left_.connect(0, buffer);
	} else {
		right_.connect(1, buffer);
	}
}

void StereoDelay::run(int start, int end) {
	updateFilter();
	for (var i = start; i < end; i++) {
		var delayedLeft = left_.run(inputs_[0][i]);
		var delayedRight = right_.run(inputs_[1][i]);

		//var fb = q + q / (1.0f - f);
		//st->ai_[0] = st->ai_[0] + f * (smp - st->ai_[0] + fb * (st->ai_[0] - st->ai_[1]));
		//smp = st->ai_[1] = st->ai_[1] + f * (st->ai_[0] - st->ai_[1]);

		var left = fltL_.run(delayedLeft);
		var right = fltR_.run(delayedRight);

		outputs_[0][i] = controls.mixLeft.value.f* left + (1.0f - controls.mixLeft.value.f) * inputs_[0][i];
		outputs_[1][i] = controls.mixRight.value.f* right + (1.0f - controls.mixRight.value.f) * inputs_[1][i];
	}
}

void StereoDelay::setDelayLeft() {
	left_.setDelay(controls.delayLeft.value.f);
}

void StereoDelay::setDelayRight() {
	right_.setDelay(controls.delayRight.value.f);
}

void StereoDelay::updateFilter() {
	fltL_.update(0.0f);
	fltR_.update(0.0f);
}

Soundbank* StereoDelay::getDefaultSoundbank() {
	return StereoDelay::defaultSoundbank_;
}

void StereoDelay::prepare() {
	var spatialLeft = NEW_(Stream);
	{
		// feedbackLeft, delayLeft
		spatialLeft->writeByte(10)->writeFloat(1.0f);
		// feedbackRight, delayRight
		spatialLeft->writeByte(20)->writeFloat(2.0f);
		// mixLeft, mixRight
		spatialLeft->writeByte(120)->writeByte(200);
		// cut, res, mod, mode
		spatialLeft->writeByte(100)->writeByte(0)->writeByte(0)->writeByte(FmLowPass | FmBandPass);
	};

	var spatialRight = NEW_(Stream);
	{
		// feedbackLeft, delayLeft
		spatialRight->writeByte(20)->writeFloat(2.0f);
		// feedbackRight, delayRight
		spatialRight->writeByte(10)->writeFloat(1.0f);
		// mixLeft, mixRight
		spatialRight->writeByte(200)->writeByte(120);
		// cut, res, mod, mode
		spatialRight->writeByte(100)->writeByte(0)->writeByte(0)->writeByte(FmLowPass | FmBandPass);
	};

	StereoDelay stereoDelay;
	StereoDelay::defaultSoundbank_ = stereoDelay.createSoundbank();
	StereoDelay::defaultSoundbank_->add("spatial left...", spatialLeft->data());
	StereoDelay::defaultSoundbank_->add("spatial right..", spatialRight->data());
	DEL_(spatialLeft);
	DEL_(spatialRight);
}

void StereoDelay::cleanUp() {
	DEL_(StereoDelay::defaultSoundbank_);
	StereoDelay::defaultSoundbank_ = NULL;
}

int StereoDelay::delayLeftSetter(void* obj, S value) {
	var that = (StereoDelay*)obj;
	PotBase::setter(that->getControl(stdlDelayLeft), value);
	that->setDelayLeft();
	return 1;
}

int StereoDelay::delayRightSetter(void* obj, S value) {
	var that = (StereoDelay*)obj;
	PotBase::setter(that->getControl(stdlDelayRight), value);
	that->setDelayRight();
	return 1;
}
