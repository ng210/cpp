//*****************************************************************************
// ADSR envelope
//*****************************************************************************

#include "Env.h"

#include <stdio.h>

NS_SSN1K_USE

NS_SSN1K_BEGIN

Env::Env(void) {
	timer_ = 0;
	phase_ = 0;
	velocity_ = 0;
	overlayCounter_ = 0;
	smp_ = 0;
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
	if (ctrls->gate.slopeUp()) {
		//if (phase_ != SSN1K_ENV_IDLE) {
		//	overlayCounter_ = OVERLAY_LENGTH;
		//}
		timer_ = 0.0f;
		phase_ = SSN1K_ENV_ATTACK;
		velocity_ = ctrls->gate.get().f;
	} else
	if (ctrls->gate.slopeDown()) {
		timer_ = ctrls->sus.get().f;
		phase_ = SSN1K_ENV_RELEASE;
	}
	ctrls->gate.update();

	if (phase_ > 0)	{
		float value;	// = ((Ctrl*)ctrls)[this->phase_ + sizeof(MdlCtrls) / sizeof(Ctrl) - 1].get().fValue;
		float rate;
		//fValue *= fValue;
		//float fRate;
		float sustain = ctrls->sus.get().f + 0.00000001f;
		float invSustain = 1.0f - sustain;
		switch (phase_) {
			case SSN1K_ENV_ATTACK: // attack
				// 0.0 : 0.005s -> 1/(0*3.995 + 0.005)/smpRate = 200/smpRate
				// 1.0 : 4.0s -> 1/(1*3.995 + 0.005)/smpRate
				//   X : Xs -> 1/(3.995*X + 0.005)/smpRate
				value = ctrls->atk.get().f;
				rate = SSN1K::getSampleRateR() / (3.995f * value + 0.005f);
				timer_ += rate;
				if (timer_ >= 1.0f) {
					phase_ = SSN1K_ENV_DECAY;
					timer_ = 1.0f;
				}
				smp_ = SSN1K::interpolate(timer_);
				break;
			case SSN1K_ENV_DECAY: // decay/sustain
				// 0.0 : 0.005s -> 1/(0*3.995 + 0.005)/smpRate = 200/smpRate
				// 1.0 : 4.0s -> 1/(1*3.995 + 0.005)/smpRate
				//   X : Xs -> 1/(3.995*X + 0.005)/smpRate
				value = ctrls->dec.get().f;
				rate = SSN1K::getSampleRateR() / (3.995f * value + 0.005f);
				timer_ -= rate;
				if (timer_ <= sustain) {
					timer_ = sustain;
					phase_ = SSN1K_ENV_SUSTAIN;
				}
				smp_ = invSustain*SSN1K::interpolate((timer_-sustain) / invSustain) + sustain;
				// todo smp = (1-fSustain)*smooth((fTimer-fSustain)/(1-fSustain)) + fSustain;
				break;
			case SSN1K_ENV_SUSTAIN:
				smp_ = sustain;
				break;
			case SSN1K_ENV_RELEASE: // release
				// 0.0 :  0.005s -> 1/(0*9.995 + 0.005)/smpRate = 200/smpRate
				// 1.0 : 10.0s -> 1/(1*9.995 + 0.005)/smpRate
				//   X :  Xs -> 1/(9.995*X + 0.005)/smpRate
				value = ctrls->rel.get().f;
				rate = SSN1K::getSampleRateR() / (9.995f * value + 0.005f);
				timer_ -= rate;
				if (timer_ <= 0.0f) {
					phase_ = SSN1K_ENV_IDLE;
					timer_ = 0.0f;
				}
				smp_ = sustain*SSN1K::interpolate(timer_/ sustain);
				break;
		}
		//overlayCounter_--;
	}
#ifdef _PROFILE
	SSN1K_PROFILER.leave(1);
#endif

	return Mdl::run(smp_, in);
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