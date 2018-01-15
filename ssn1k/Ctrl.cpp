#include "Ctrl.h"

NS_SSN1K_USE

NS_SSN1K_BEGIN

#ifdef _PROFILE
_Profiler SSN1K_PROFILER;

void SSN1K_ProfileInit() {
	SSN1K_PROFILER.init("SSN1K");
	SSN1K_PROFILER.addName(1, "Env");
	SSN1K_PROFILER.addName(2, "Osc");
	SSN1K_PROFILER.addName(3, "Flt");
	SSN1K_PROFILER.addName(4, "Mdl.p2f");
}

#endif

CtrlValue Ctrl::get() {
	return value_[0];
}
void Ctrl::update() {
	value_[1] = value_[0];
}
int Ctrl::slopeUp() {
	return value_[1].i <= 0 && value_[0].i > 0;
}
int Ctrl::slopeDown() {
	return value_[0].i <= 0 && value_[1].i > 0;
}
void Ctrl::set(int value) {
	value_[0].i = value;
}
bool Ctrl::set(float value) {
	set(0.0f, 0.0f, 1.0f);
	return set(value, true);
}
void Ctrl::set(float value, float min, float max) {
	min_.f = min;
	max_.f = max;
	set(value, true);
}
bool Ctrl::set(float value, bool check) {
	int isValid = 0;
	if (check) {
		isValid = value >= min_.f;
		if (!isValid) {
			value = min_.f;
		} else {
			isValid = value < max_.f;
			if (!isValid) value = max_.f;
		}
	}
	value_[0].f = value;
	return isValid;
}
bool Ctrl::change() {
	return set(value_[0].f + delta_, true);
}
bool Ctrl::change(float value) {
	return set(value_[0].f + value, true);
}

NS_SSN1K_END