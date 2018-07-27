//*****************************************************************************
// ADSR envelope
//*****************************************************************************

//#include <stdio.h>
#include "env.h"

NS_SSN1K_USE

NS_SSN1K_BEGIN

Env::Env(void) {
	ticksPerSample_ = NULL;
	reset();
}

int Env::isActive() {
	return phase_ != SSN1K_ENV_IDLE;
}

void Env::setGate(float v) {
	EnvCtrls* ctrls = (EnvCtrls*)controls_;
	if (gate_ == 0) {
		if (v > 0.0f) {
			// slope up: retrigger envelope
			gate_ = 1;
			//timer_ = 0.0f;
			phase_ = SSN1K_ENV_ATTACK;
			velocity_ = v;
			float value = ctrls->atk.f + SSN1K::getSampleRateR();
			rate_ = *ticksPerSample_ / value;
		}
	} else {
		if (v <= 0.0f) {
			// slope down: start release phase
			gate_ = 0;
			//timer_ = ctrls->sus.f + SSN1K::getSampleRateR();
			phase_ = SSN1K_ENV_RELEASE;
			float value = ctrls->rel.f + SSN1K::getSampleRateR();
			float sustain = ctrls->sus.f + SSN1K::getSampleRateR();
			rate_ = -*ticksPerSample_ / value * (sustain + SSN1K::getSampleRateR());
		}
	}
}

void Env::reset() {
	phase_ = 0;
	gate_ = 0;
	timer_ = 0.0f;
	rate_ = 0.0f;
	velocity_ = 0.0f;
	//overlayCounter_ = 0;
	smp_ = 0.0f;
	phase_ = SSN1K_ENV_IDLE;
}
float Env::run() {
#ifdef _PROFILE
	SSN1K_PROFILER.enter(1);
#endif
	EnvCtrls* ctrls = (EnvCtrls*)controls_;

	float sustain = ctrls->sus.f + SSN1K::getSampleRateR();
	float invSustain = 1.0f - sustain;

	if (phase_ != SSN1K_ENV_IDLE) {
		timer_ += rate_;
		switch (phase_) {
			case SSN1K_ENV_ATTACK: // attack
				if (timer_ < 1.0f) {
					smp_ = SSN1K::interpolate(timer_, 0.0f, 1.0f);
					break;
				} else {
					phase_ = SSN1K_ENV_DECAY;
					timer_ = 1.0f;
					float value = ctrls->dec.f + SSN1K::getSampleRateR();
					rate_ = -*ticksPerSample_ / value * invSustain;
				}
			case SSN1K_ENV_DECAY: // decay/sustain
				// 0.0 : 0.005s -> 1/(0*3.995 + 0.005)/smpRate = 200/smpRate
				// 1.0 : 4.0s -> 1/(1*3.995 + 0.005)/smpRate
				//   X : Xs -> 1/(3.995*X + 0.005)/smpRate
				if (timer_ > sustain) {
					smp_ = SSN1K::interpolate(timer_, sustain, 1.0f);
				} else {
					timer_ = sustain;
					smp_ = sustain;
				}
				break;
			case SSN1K_ENV_RELEASE: // release
				if (timer_ > 0.0f) {
					smp_ = SSN1K::interpolate(timer_, 0.0f, sustain);
				} else {
					phase_ = SSN1K_ENV_IDLE;
					timer_ = 0.0f;
					smp_ = 0.0f;
				}
				break;
			default:
				break;
		}
		// mod=0.0 output=smp
		// mod=1.0 output=velocity*smp
		// mod=0.5 output=smp*(0.5+0.5*velocity)
		// output=smp*(1.0-mod + mod*velocity)
		float mod = ctrls->mix.f;
		smp_ *= ctrls->amp.f * (1.0f - mod + mod * velocity_);
		smp_ += ctrls->dc.f;
	}
#ifdef _PROFILE
	SSN1K_PROFILER.leave(1);
#endif
	return smp_;
}
void Env::ticksPerSample(float* v) {
	ticksPerSample_ = v;
}

NS_SSN1K_END