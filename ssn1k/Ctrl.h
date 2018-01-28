//*****************************************************************************
// Control entity
//*****************************************************************************

#ifndef __CTRL_H
#define __CTRL_H

#include "ssn1k.h"

NS_SSN1K_BEGIN

#ifdef _PROFILE

#include "profilec.h"

extern _Profiler SSN1K_PROFILER;

void SSN1K_ProfileInit();
#endif

union CtrlValue {
		float f;
		int i;
		CtrlValue() : i(0) {}
		CtrlValue(int v) : i(v) {}
		CtrlValue(float v) : f(v) {}
};

typedef struct CtrlSetting_ {
	int id;
	CtrlValue value;
} CtrlSetting;

class Ctrl {
	CtrlValue value_;
	CtrlValue min_;
	CtrlValue max_;

	float delta_;
	//int duration_;

public:
	CtrlValue get();

	bool change();
	bool change(float);
	//void set(int);
	void set(CtrlValue);
	bool set(float);
	bool set(float, bool);
	void set(float, float, float);
};

NS_SSN1K_END

#endif