//*****************************************************************************
// 2 pole Butterworth filter
//*****************************************************************************

#include <math.h>
#include "flt.h"

NS_SSN1K_BEGIN

Flt::Flt() {
	reset();
}

void Flt::reset() {
	float* dst = &this->ai[0];
	for (int i = 0; i < 3 * 3 + 4 * 2; i++) {
		dst[i] = 0.0f;
	}
}

float Flt::run(float in, float cut) {
#ifdef _PROFILE
	SSN1K_PROFILER.enter(3);
#endif
	FltCtrls* ctrls = (FltCtrls*)controls_;
	int mode = ctrls->mode.i();
	float out = in;
	if (mode != 0) {
		// Update filter coefficients
		float res = (ctrls->res.i() == 0) ? 1.0f : 1.0f - ctrls->res.f();
		float e = cut * SSN1K::getTheta();
		//float e = (float)tan(cut*SSN1K::getTheta());
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

NS_SSN1K_END