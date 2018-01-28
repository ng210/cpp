//*****************************************************************************
// Base module
//*****************************************************************************

#include "Mdl.h"
#include "base/MemoryMgr.h"

//double SSN1K_THETA = 2*M_PI;

NS_FW_BASE_USE
NS_SSN1K_BEGIN

float Mdl::run(float in1, float in2) {
	switch (mix_->get().i) {
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
	smp_ = amp_->get().f * smp_ + dc_->get().f;
	return smp_;
}
void Mdl::setControls(MdlCtrls* controls) {
	mix_ = controls->mix;
	amp_ = controls->amp;
	dc_ = controls->dc;
}

NS_SSN1K_END
