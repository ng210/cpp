#include <math.h>
#include "mixer.h"
#include "base/memory.h"

NS_SSN1K_BEGIN

Mixer::Mixer() {
	inputCount_ = 0;
	controls_ = Mdl::createControls(SSN1K_CI_MIX_COUNT);
}

Mixer::~Mixer() {
	destroyControls();
}

void Mixer::writeSample(short* ptr, float smp) {
	int output = (int)round(32767.0f * smp);
	if (output > -32767) {
		if (output < 32767) {
			*ptr = output;
		} else {
			*ptr = 32767;
		}
	} else {
		*ptr = -32767;
	}
}

float Mixer::run(void* buffer, size_t start, size_t end) {
	MixCtrls* ctrls = (MixCtrls*)controls_;
	for (size_t i = start; i<end; i++) {
		float left = 0.0f;
		float right = 0.0f;
		MixCtrls* ctrls = (MixCtrls*)controls_;
		for (int j = 0; j < inputCount_; j++) {
			if (ctrls->volume[j].i == 0) {
				continue;
			}
			float smp = input_[j]->run() * ctrls->volume[j].f;
			float balance = (ctrls->balance[j].f + input_[j]->getControl(SSN1K_CI_SynthBal)->f) / 2.0f;
			float lSmp = smp * balance;
			left += lSmp;
			right += smp - lSmp;
		}
		short* ptr = &((short*)buffer)[2 * i];
		writeSample(ptr, left);
		writeSample(ptr+1, right);
			}
	return 0.0f;
}

void Mixer::addInput(Synth* in) {
	if (inputCount_ < 16) {
		//MixCtrls* ctrls = (MixCtrls*)controls_;
		input_[inputCount_] = in;
		inputCount_++;
	}
}

void Mixer::setControls(UINT8* data) {
	UINT8* ptr = data;
	int ctrlId;
	while ((ctrlId = *ptr++) != 0xFF) {
		Ctrl* ctrl = &controls_[ctrlId];
		if (ctrlId == SSN1K_CI_MixMix) {
			ctrl->i = *ptr++;
		} else
		if (ctrlId < SSN1K_CI_MIX_COUNT) {
			ctrl->f = *ptr++ / 255.0f;
		}
	}
}

NS_SSN1K_END
