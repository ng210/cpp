//*****************************************************************************
// Base module
//*****************************************************************************

#ifndef __MDL_H
#define __MDL_H

#define _USE_MATH_DEFINES
#include <math.h>

#include "ssn1k.h"
#include "Ctrl.h"

//extern double SSN1K_THETA;

NS_SSN1K_BEGIN

struct MdlCtrls {
	Ctrl mix;
	Ctrl amp;
	Ctrl dc;
};

enum SSN1K_MixMode {
	 SSN1K_MM_OVR	= 0x00	// overwrite
	,SSN1K_MM_ADD	= 0x01	// add
	,SSN1K_MM_MUL	= 0x02	// multiply
	,SSN1K_MM_BPS	= 0x03	// bypass

	,SSN1K_MM_COUNT	
};

class Mdl {
protected:
	Ctrl* ctrls_;
	float run(float, float);
public:
	void setCtrls(Ctrl*);
	float smp_;
};

NS_SSN1K_END

#endif