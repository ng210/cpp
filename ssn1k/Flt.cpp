//*****************************************************************************
// 2 pole Butterworth filter
//*****************************************************************************

#include "Flt.h"
#include <math.h>

NS_SSN1K_BEGIN

Flt::Flt() {
	for (int i=0; i<3*3+4*2; i++) {
		((float*)&this->ai)[i] = 0.0f;
	}
}

float Flt::run(float in, float cut) {
#ifdef _PROFILE
	SSN1K_PROFILER.enter(3);
#endif

	int mode = mode_->get().i;
	float out = in;
	if (mode != 0) {
		// Update filter coefficients
		//float cut = this->env.run(ctrls->env, 0.0)*SSN1K_MIXING_RATE/8;
		//float cut = 10.0f + (SSN1K_smpRate - 10) * fCutMod;
		//fCutMod = 100/SSN1K_smpRate;
		//float cut = fCutMod*fCutMod/4/4 + 10*SSN1K::getSampleRateR();
		float res = (res_->get().i == 0) ? 1.0f : 1.0f - res_->get().f;
		float e = (float)tan(cut*SSN1K::getTheta());
		float g = (float)(-/*M_SQRT2 **/ res * e);
		float b0 = e*e;
		this->bi[0] = this->bi[1] = b0; this->bi[2] = 2*b0;
		this->ci[0] = this->ci[2] = 1; this->ci[1] = -2;
		this->ai[0] = b0 + 1.0f - g;
		this->ai[2] = b0 + 1.0f + g;
		this->ai[1] = 2*(b0 - 1);

		// Apply filter
		float gain = 1.0f/this->ai[0];
		float lp = (this->bi[0]*in + this->bi[1]*this->ui[0] + this->bi[2]*this->ui[1] - this->ai[1]*this->lp[0] - this->ai[2]*this->lp[1])*gain;
		float hp = (this->ci[0]*in + this->ci[1]*this->vi[0] + this->ci[2]*this->vi[1] - this->ai[1]*this->hp[0] - this->ai[2]*this->hp[1])*gain;
		// lp inputs
		this->ui[1] = this->ui[0]; this->ui[0] = in;
		// hp inputs
		this->vi[1] = this->vi[0]; this->vi[0] = in;
		// lp outputs
		this->lp[1] = this->lp[0]; this->lp[0] = lp;
		// hp outputs
		this->hp[1] = this->hp[0]; this->hp[0] = hp;

		out = 0.0f;
		if (mode & SSN1K_FM_LP) // lowpass
			out += lp;
		if (mode & SSN1K_FM_HP) // hipass
			out += hp;
	}
//	else
//		return in;
#ifdef _PROFILE
	SSN1K_PROFILER.leave(3);
#endif
	return out;
}

void Flt::setControls(FltCtrls* controls) {
	Mdl::setControls(controls);
	res_ = controls->res;
	mode_ = controls->mode;
}

NS_SSN1K_END