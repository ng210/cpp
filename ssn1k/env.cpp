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
			float value = ctrls->atk.f() + SSN1K::getSampleRateR();
			rate_ = *ticksPerSample_ / value;
		}
	} else {
		if (v <= 0.0f) {
			// slope down: start release phase
			gate_ = 0;
			//timer_ = ctrls->sus.f() + SSN1K::getSampleRateR();
			phase_ = SSN1K_ENV_RELEASE;
			float value = ctrls->rel.f() + SSN1K::getSampleRateR();
			float sustain = ctrls->sus.f() + SSN1K::getSampleRateR();
			rate_ = *ticksPerSample_ / value * (sustain + SSN1K::getSampleRateR());
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

	float sustain = ctrls->sus.f() + SSN1K::getSampleRateR();
	float invSustain = 1.0f - sustain;

	if (phase_ != SSN1K_ENV_IDLE)	{
		switch (phase_) {
			case SSN1K_ENV_ATTACK: // attack
				timer_ += rate_;
				if (timer_ >= 1.0f) {
					phase_ = SSN1K_ENV_DECAY;
					timer_ = 1.0f;
					float value = ctrls->dec.f() + SSN1K::getSampleRateR();
					rate_ = *ticksPerSample_ / value * invSustain;
				} else {
					smp_ = SSN1K::interpolate(timer_);
					break;
				}
			case SSN1K_ENV_DECAY: // decay/sustain
				// 0.0 : 0.005s -> 1/(0*3.995 + 0.005)/smpRate = 200/smpRate
				// 1.0 : 4.0s -> 1/(1*3.995 + 0.005)/smpRate
				//   X : Xs -> 1/(3.995*X + 0.005)/smpRate
				timer_ -= rate_;
				if (timer_ <= sustain) {
					timer_ = sustain;
					smp_ = sustain;
					//phase_ = SSN1K_ENV_SUSTAIN;
				} else {
					//smp_ = invSustain*SSN1K::interpolate((timer_ - sustain) / invSustain) + sustain;
					smp_ = SSN1K::interpolate(timer_);
				}
				break;
			case SSN1K_ENV_RELEASE: // release
				timer_ -= rate_;
				if (timer_ <= 0.0f) {
					phase_ = SSN1K_ENV_IDLE;
					timer_ = 0.0f;
				}
				//smp_ = sustain*SSN1K::interpolate(timer_/ sustain);
				smp_ = SSN1K::interpolate(timer_);
				break;
			default:
				break;
		}
	}
#ifdef _PROFILE
	SSN1K_PROFILER.leave(1);
#endif
	// mod=0.0 out=smp
	// mod=1.0 out=velocity*smp
	// mod=0.5 out=smp*(0.5+0.5*velocity)
	// out=smp*(1.0-mod + mod*velocity)
	float mod = ctrls->mix.f();
	//return Mdl::mix(smp_ * (1.0f - mod + mod * velocity_), 0.0f);
	smp_ *= ctrls->amp.f() * (1.0f - mod + mod * velocity_);
	return smp_ + ctrls->dc.f();
}
void Env::ticksPerSample(float* v) {
	ticksPerSample_ = v;
}
/*
float Env::run(EnvCtrls& ctrls, float in)
{
	float out = 0.0f;
	if (ctrls->gate.slopeUp())
	{
		fTimer = 1.0f;
		iPhase = 1;
		fVelocity = ctrls->gate.f()Value;
	}
	else
	if (ctrls->gate.slopeDown())
	{
		fTimer = 1.0f;
		iPhase = 4;
	}
	ctrls->gate.update();
	if (this->iPhase != 0)
	{
		float fValue = ((Ctrl*)&ctrls)[this->iPhase].f()Value;
		float fTimer2 = fTimer * fTimer;
		if (this->iPhase != 3)
		{ // not sustain
			float fRate = (fValue == 0) ? 1.0f : 1.0f/SSN1K_MIXING_RATE/fValue;
			switch (this->iPhase)
			{
				case 1: // atk
					out = smp = 1.0f - fTimer;
					break;
				case 2: // dec
					out = smp = fTimer2 * (1.0f - ctrls->sus.f()Value) + ctrls->sus.f()Value;
					break;
				case 4: // rel
					out = fTimer2 * smp;
					break;
			}
			if (fTimer <= 0.0f)
			{
				iPhase++;
				iPhase %= 5;
				fTimer = 1.0f;
			} else
			{
				fTimer -= fRate;
			}
		} else
		{ // sustain
			out = smp = fValue;
		}
		out *= fVelocity * ctrls->amp.f()Value;
	}
	return mix(ctrls, in, out + ctrls->dc.f()Value);
}
*/

NS_SSN1K_END