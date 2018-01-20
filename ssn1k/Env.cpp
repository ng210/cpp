//*****************************************************************************
// ADSR envelope
//*****************************************************************************

#include "Env.h"

#include <stdio.h>

NS_SSN1K_USE

NS_SSN1K_BEGIN

Env::Env(void) {
	bpm(60);
	phase_ = 0;
	timer_ = 0.0f;
	rate_ = 0.0f;
	velocity_ = 0.0f;
	//overlayCounter_ = 0;
	smp_ = 0.0f;
}

int Env::isActive() {
	return phase_ != SSN1K_ENV_IDLE;
}

void Env::slopeUp() {
	((EnvCtrls*)ctrls_)->gate.set(0.0f);
	((EnvCtrls*)ctrls_)->gate.set(0.5f);
}
void Env::slopeDown() {
	((EnvCtrls*)ctrls_)->gate.set(0.5f);
	((EnvCtrls*)ctrls_)->gate.set(0.0f);
}

float Env::run(float in) {
#ifdef _PROFILE
	SSN1K_PROFILER.enter(1);
#endif
	EnvCtrls* ctrls = (EnvCtrls*)ctrls_;
	float sustain = ctrls->sus.get().f + 0.00000001f;
	float invSustain = 1.0f - sustain;

	if (ctrls->gate.slopeUp()) {
		timer_ = 0.0f;
		phase_ = SSN1K_ENV_ATTACK;
		velocity_ = ctrls->gate.get().f;
		float value = ctrls->atk.get().f;
		rate_ = tickPerSample_ / value;
	} else
	if (ctrls->gate.slopeDown()) {
		timer_ = ctrls->sus.get().f;
		phase_ = SSN1K_ENV_RELEASE;
		float value = ctrls->rel.get().f;
		rate_ = tickPerSample_ / value * sustain;
	}
	ctrls->gate.update();

	if (phase_ > 0)	{
		switch (phase_) {
			case SSN1K_ENV_ATTACK: // attack
				timer_ += rate_;
				if (timer_ >= 1.0f) {
					phase_ = SSN1K_ENV_DECAY;
					timer_ = 1.0f;
					float value = ctrls->dec.get().f;
					rate_ = tickPerSample_ / value * invSustain;
				}
				smp_ = SSN1K::interpolate(timer_);
				break;
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
					smp_ = invSustain*SSN1K::interpolate((timer_ - sustain) / invSustain) + sustain;
				}
				break;
			case SSN1K_ENV_RELEASE: // release
				timer_ -= rate_;
				if (timer_ <= 0.0f) {
					phase_ = SSN1K_ENV_IDLE;
					timer_ = 0.0f;
				}
				smp_ = sustain*SSN1K::interpolate(timer_/ sustain);
				break;
		}
	}
#ifdef _PROFILE
	SSN1K_PROFILER.leave(1);
#endif

	return Mdl::run(smp_, in);
}
void Env::bpm(float v) {
	bpm_ = v;
	tickPerSample_ = v * SSN1K::getSampleRateR() / 60;
}

/*
float Env::run(EnvCtrls& ctrls, float in)
{
	float out = 0.0f;
	if (ctrls->gate.slopeUp())
	{
		fTimer = 1.0f;
		iPhase = 1;
		fVelocity = ctrls->gate.get().fValue;
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
		float fValue = ((Ctrl*)&ctrls)[this->iPhase].get().fValue;
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
					out = smp = fTimer2 * (1.0f - ctrls->sus.get().fValue) + ctrls->sus.get().fValue;
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
		out *= fVelocity * ctrls->amp.get().fValue;
	}
	return mix(ctrls, in, out + ctrls->dc.get().fValue);
}
*/

NS_SSN1K_END