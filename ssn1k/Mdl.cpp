//*****************************************************************************
// Base module
//*****************************************************************************

#include "Mdl.h"

//double SSN1K_THETA = 2*M_PI;

NS_SSN1K_BEGIN

float Mdl::run(float in1, float in2) {
	MdlCtrls* ctrls = (MdlCtrls*)ctrls_;
	switch (ctrls->mix.get().i) {
		case SSN1K_MM_ADD:
			smp_ = in1 + in2;
			break;
		case SSN1K_MM_MUL:
			smp_ = in1 * in2;
			break;
		case SSN1K_MM_BPS:
			smp_ = in2;
			break;
		case SSN1K_MM_OVR:
		default:
			smp_ = in1;
			break;
	}
	smp_ = ctrls->amp.get().f * smp_ + ctrls->dc.get().f;
	return smp_;
}
void Mdl::setCtrls(Ctrl* ctrls) {
	ctrls_ = ctrls;
}
float sinusoid(float x) {
	return (float)(1.0f - cos(x * M_PI)) / 2;
}
float smoothstep(float x) {
	return x*x*(3 - 2*x);
}
float flat(float x) {
	return x;
}

NS_SSN1K_END
