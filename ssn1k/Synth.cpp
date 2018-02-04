//*****************************************************************************
// Simple synth: in->osc->env->flt->out
//*****************************************************************************

#include "base/MemoryMgr.h"
#include "Synth.h"
#include "Voice.h"
#include <math.h>

NS_FW_BASE_USE
NS_SSN1K_BEGIN

//SynthCtrls::SynthCtrls() {
//	Ctrl* ctrl = (Ctrl*)this;
//	for (int i=0; i<SSN1K_CI_COUNT; i++) {
//		ctrl[i].set(.0f, .0f, 1.f);
//	}
//	//this->osc2.mix.set(SSN1K_MM_BPS);
//	//this->flt.mix.set(SSN1K_MM_BPS);
//	this->osc1.mix.set(SSN1K_MM_MUL);
//}

Synth::Synth(size_t voices) {
	init(voices);
}

void Synth::init(size_t voices) {
	smp_ = .0f;
	voiceCount_ = voices > 64 ? 64 : voices;
	//overlayValue_ = .0f;
	// create controls
	createControls(SSN1K_CI_COUNT);
	//sweepCtrls = 0;
	//paSweepCtrls = new Ctrl*[256];
	// add voices
	for (int i = 0; i < voiceCount_; i++) {
		voices_[i] = NEW_(Voice, this, i);
	}
	nextVoice_ = 0;
#ifdef _PROFILE

	profiler.init("Synth");
	profiler.addName(1, "ctrlSweep");
	profiler.addName(2, "lfo");
	profiler.addName(3, "env");
	profiler.addName(4, "osc");
	profiler.addName(5, "flt");
	profiler.addName(6, "overlay");

#endif
}
Synth::~Synth() {
	//delete paSweepCtrls;
	// delete voices
	for (size_t i = 0; i < voiceCount_; i++) {
		DEL_(voices_[i]);
	}
	FREE(controls_[0]);
	FREE(controls_);
}

float Synth::run() {
	float smp = 0.0f;
	for (size_t j = 0; j<voiceCount_; j++) {
		if (voices_[j]->isActive()) {
			smp += voices_[j]->run();
		}
	}
	return smp;
}

void Synth::run(void* buffer, size_t start, size_t end) {
	for (size_t i = start; i<end; i++) {
		float smp = 0.0f;
		for (size_t j = 0; j<voiceCount_; j++) {
			if (voices_[j]->isActive()) {
				smp += voices_[j]->run();
			}
		}
		short out;
		smp *= controls_[SSN1K_CI_SynthAmp]->get().f;
		if (smp < 1.0f) {
			if (smp > -1.0f) {
				out = (short)round(32767.0f * smp);
			} else {
				out = -32767;
			}
		} else {
			out = 32767;
		}
		short left = (short)(controls_[SSN1K_CI_SynthBal]->get().f * out);
		short right = (short)((1.0f - controls_[SSN1K_CI_SynthBal]->get().f) * out);
		((short*)buffer)[2 * i] += left;
		((short*)buffer)[2 * i + 1] += right;
	}
}
void Synth::setNote(int note, int duration, float velocity) {
	// get free voice
	//printf("synth: %llx, note: %d, velocity: %f, duration: %d\n", this, note, velocity, duration);
	Voice* voice = voices_[nextVoice_];
	nextVoice_ = (nextVoice_ + 1) % voiceCount_;
	voice->setNote(note, duration, velocity);
}

//void Synth::setCtrlSweep(int id, float fStart, float fEnd, int iCycles) {
//	Ctrl* ctrl = paSweepCtrls[iSweepCtrls] = &ctrls[id];
//	ctrl->duration = iCycles;
//	ctrl->set(fStart, 0);
//	ctrls->delta = (fEnd - fStart)/iCycles;
//	iSweepCtrls++;
//}

NS_SSN1K_END