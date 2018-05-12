//*****************************************************************************
// Base module
//*****************************************************************************

#include "mdl.h"
#include "base/memory.h"

//double SSN1K_THETA = 2*M_PI;

NS_FW_BASE_USE
NS_SSN1K_BEGIN

float Mdl::mix(float in1, float in2) {
	MdlCtrls* ctrls = (MdlCtrls*)controls_;
	float smp = in1*ctrls->amp.f() + ctrls->dc.f();
	switch (ctrls->mix.i()) {
		case SSN1K_MM_ADD:
			smp = 0.5f * (smp + in2);
			break;
		case SSN1K_MM_MUL:
			smp *= in2;
			break;
		case SSN1K_MM_BPS:
			smp = in2;
			break;
		case SSN1K_MM_OVR:
		default:
			break;
	}
	return smp;
}

void Mdl::destroyControls() {
	FREE(controls_);
}

//void Mdl::setup(CtrlSetting* data) {
//	Ctrl::setControls(controls_, data);
//}

Ctrl* Mdl::getControl(size_t id) {
	return &controls_[id];
}

NS_SSN1K_END
