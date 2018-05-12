//*****************************************************************************
// Control entity
//*****************************************************************************

#ifndef __CTRL_H
#define __CTRL_H

#include "ssn1k.h"
#include "basedef.h"

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

//typedef struct CtrlSetting_ {
//	int id;
//	CtrlValue value;
//} CtrlSetting;

class Ctrl {
	CtrlValue value_;
	//CtrlValue min_;
	//CtrlValue max_;
	//float delta_;
	//int duration_;
public:
	//bool change();
	//bool change(float);
	////void set(int);
	void set(CtrlValue value) { value_ = value; }
	void set(float value) { value_.f = value; }
	void set(int value) { value_.i = value; }
	int i() { return value_.i; };
	float f() { return value_.f; };
	//bool set(float, bool);
	//void set(float, float, float);

	static Ctrl* createControls(int count);
	//static void setControls(Ctrl** controls, CtrlSetting* data);
};

NS_SSN1K_END

#endif