//*****************************************************************************
// Multi waveform oscillator
//*****************************************************************************

#include <math.h>
#include "Osc.h"

NS_SSN1K_BEGIN

Osc::Osc(void) {
	timer_ = 0;
	noiseTimer_ = 0;
	//fDelta = 0;
}

float Osc::run(float am, float fm, float pm) {
#ifdef _PROFILE
	SSN1K_PROFILER.enter(2);
#endif

	float pitch = (float)(note_->get().f + tune_->get().f);
	float delta = (fm + fre_->get().f + SSN1K::p2f(pitch)) * SSN1K::getSampleRateR();
	if (delta >= 1.0f) {
		delta = 0.99999999f;
	}
	float psw = pm + psw_->get().f;
	float out = 0.0f;
	int waveForm = wav_->get().i & 0x1F;
	int waveFormCount = 0;
	if (waveForm & SSN1K_WF_TRI) {
		float tmp = (timer_ < psw) ? timer_/psw : (1.0f - timer_)/(1.0f - psw);
		out += 2*tmp - 1.0f;
		waveFormCount++;
	}
	if (waveForm & SSN1K_WF_PSAW) {
		out += (timer_ < psw) ? 2.0f * timer_/psw : .0f;
		out -= 1.0f;
		waveFormCount++;
	}
	if (waveForm & SSN1K_WF_PLS) {
		out += timer_ < psw ? 1.0f : -1.0f;
		waveFormCount++;
	}
	if (waveForm & SSN1K_WF_RND) {
		if (timer_ < delta || timer_ > 0.5f && timer_ < 0.5f + delta) {
			smp_ = SSN1K::rnd();
		}
		out = smp_;
		waveFormCount++;
	}
	if (waveForm & SSN1K_WF_SIN) {
		float arg = (float)(SSN1K::get2PI() * timer_);
		out += (float)sin(arg);
		waveFormCount++;
	}
	if (waveFormCount > 1) {
		out /= waveFormCount;
	}
	if ((timer_ += delta) > 1.0f) {
		timer_ -= 1.0f;
	}
	if ((noiseTimer_ += 2*delta) > 1.0f) {
		noiseTimer_ -= 1.0f;
	}
#ifdef _PROFILE
	SSN1K_PROFILER.leave(2);
#endif

	return Mdl::run(out, am);
}

void Osc::setControls(OscCtrls* controls) {
	Mdl::setControls(controls);
	note_ = controls->note;
	tune_ = controls->tune;
	fre_ = controls->fre;
	psw_ = controls->psw;
	wav_ = controls->wav;
}

NS_SSN1K_END