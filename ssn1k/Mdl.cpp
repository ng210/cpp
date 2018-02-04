//*****************************************************************************
// Base module
//*****************************************************************************

#include "Mdl.h"
#include "base/MemoryMgr.h"

//double SSN1K_THETA = 2*M_PI;

NS_FW_BASE_USE
NS_SSN1K_BEGIN

float Mdl::mix(float in1, float in2) {
	MdlCtrls* ctrls = (MdlCtrls*)controls_;
	float smp_ = ctrls->amp->get().f*in1 + ctrls->dc->get().f;
	switch (ctrls->mix->get().i) {
		case SSN1K_MM_ADD:
			smp_ += in2;
			break;
		case SSN1K_MM_MUL:
			smp_ *= in2;
			break;
		case SSN1K_MM_BPS:
			smp_ = in2;
			break;
		case SSN1K_MM_OVR:
		default:
			break;
	}
	return smp_;
}

void Mdl::createControls(int count) {
	controls_ = (Ctrl**)MALLOC(Ctrl*, count);
	Ctrl* controls = MALLOC(Ctrl, count);
	for (int i = 0; i < count; i++) {
		controls_[i] = &controls[i];
		controls[i].set(0.0, 0.0, 1.0f);
	}
}

void Mdl::setup(CtrlSetting* data) {
	while (data->id != -1) {
		Ctrl* ctrl = getControl(data->id);
		ctrl->set(data->value);
		data++;
	}
}

Ctrl* Mdl::getControl(size_t id) {
	return controls_[id];
}

NS_SSN1K_END
