//*****************************************************************************
// Multi waveform oscillator
//*****************************************************************************

#include <math.h>
#include "osc.h"

NS_SSN1K_BEGIN

Osc::Osc(void) {
	timer_ = 0;
	noiseTimer_ = 0;
	smp_ = 0.0f;
	//fDelta = 0;
}

float Osc::run(float amp, float tune, float freq, float phase, float in) {
	OscCtrls* ctrls = (OscCtrls*)controls_;

#ifdef _PROFILE
	SSN1K_PROFILER.enter(2);
#endif

	float pitch = (float)(tune + ctrls->note.f() + ctrls->tune.f());
	float delta = (freq + ctrls->fre.f() + SSN1K::p2f(pitch)) * SSN1K::getSampleRateR();
	if (delta >= 1.0f) {
		delta = 0.99999999f;
	}
	float psw = phase + ctrls->psw.f();
	float out = 0.0f;
	int waveForm = ctrls->wav.i() & 0x1F;
	int waveFormCount = 0;
	if (waveForm & SSN1K_WF_TRI) {
		float tmp = (timer_ < psw) ? timer_/psw : (1.0f - timer_)/(1.0f - psw);
		//float tmp = (timer_ < psw) ? timer_/psw : timer_/(psw - 1.0f);
		out += 2 * tmp - 1.0f;
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

	return Mdl::mix(amp*out, in);
}

NS_SSN1K_END