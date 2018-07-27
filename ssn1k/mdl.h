//*****************************************************************************
// Base module
//*****************************************************************************

#ifndef __MDL_H
#define __MDL_H

#define _USE_MATH_DEFINES

#include "basedef.h"
#include "ssn1k.h"
#include <stdio.h>

NS_SSN1K_BEGIN

typedef union CtrlUnion {
	float f;
	int i;
	CtrlUnion() : i(0) {}
	CtrlUnion(int v) : i(v) {}
	CtrlUnion(float v) : f(v) {}
} CtrlUnion;

typedef CtrlUnion Ctrl;

struct MdlCtrls {
	Ctrl mix;
	Ctrl amp;
	Ctrl dc;
};

enum SSN1K_MixMode : UINT8 {
	 SSN1K_MM_OVR	= 0x00	// overwrite
	,SSN1K_MM_ADD	= 0x01	// add
	,SSN1K_MM_MUL	= 0x02	// multiply
	,SSN1K_MM_BPS	= 0x03	// bypass
	,SSN1K_MM_MIX	= 0x04	// mix

	,SSN1K_MM_COUNT	
};

class Mdl {
protected:
	float smp_;
	float mix(Ctrl in1, Ctrl in2);
	void destroyControls();
public:
	Ctrl* controls_;
	Ctrl* getControl(size_t id);

	static Ctrl* createControls(size_t count);
};

NS_SSN1K_END

#endif