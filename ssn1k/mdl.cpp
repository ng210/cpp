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
	float smp = ctrls->amp->get().f*in1 + ctrls->dc->get().f;
	switch (ctrls->mix->get().i) {
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

void Mdl::createControls(int count) {
	controls_ = (Ctrl**)MALLOC(Ctrl*, count);
	Ctrl* controls = MALLOC(Ctrl, count);
	for (int i = 0; i < count; i++) {
		controls_[i] = &controls[i];
		controls[i].set(0.0, 0.0, 1.0f);
	}
}

void Mdl::destroyControls() {
	FREE(controls_[0]);
	FREE(controls_);
}

void Mdl::setup(CtrlSetting* data) {
	while (data->id != -1) {
		Ctrl* ctrl = getControl(data->id);
		ctrl->set(data->value);
		data++;
	}
}

void Mdl::setup(BYTE* data) {
	int id = *data++;
	while (*data != -1) {
		Ctrl* ctrl = getControl(id);
		ctrl->set(*data/128.0f);
		data++;
	}
}

Ctrl* Mdl::getControl(size_t id) {
	return controls_[id];
}

NS_SSN1K_END
