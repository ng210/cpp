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

typedef union __CtrlValue {
		float f;
		int i;
} CtrlValue;

class Ctrl {
	CtrlValue value_[2];
	CtrlValue min_;
	CtrlValue max_;

	float delta_;
	//int duration_;

public:
	CtrlValue get();
	void update();
	int slopeUp();
	int slopeDown();

	bool change();
	bool change(float);
	void set(int);
	bool set(float);
	bool set(float, bool);
	void set(float, float, float);
};

NS_SSN1K_END

#endif