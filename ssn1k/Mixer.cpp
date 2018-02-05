#include "Mixer.h"
#include "base/MemoryMgr.h"
#include <math.h>

NS_FW_BASE_USE
NS_SSN1K_BEGIN

Mixer::Mixer() {
	inputCount_ = 0;
	createControls(SSN1K_CI_MIX_COUNT);
}

Mixer::~Mixer() {
	FREE(controls_[0]);
	FREE(controls_);
}

void Mixer::writeSample(short* ptr, float smp) {
	int out = (int)round(32767.0f * smp);
	if (out > -32767) {
		if (out < 32767) {
			*ptr = out;
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
		for (size_t j = 0; j < inputCount_; j++) {
			float smp = input_[j]->run() * ctrls->volume[j]->get().f;
			float balance = (ctrls->balance[j]->get().f + input_[j]->getControl(SSN1K_CI_SynthBal)->get().f) / 2.0f;
			float lSmp = smp * balance;
			left += lSmp;
			right += smp - lSmp;
		}
		short* ptr = &((short*)buffer)[2 * i];
		writeSample(ptr, left);
		writeSample(ptr+1, right);

		//((short*)buffer)[2 * i] += (short)(smp * ctrls->balance[j]->get().f);
		//((short*)buffer)[2 * i + 1] += (short)(smp * (1.0f - ctrls->balance[j]->get().f));
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

NS_SSN1K_END
