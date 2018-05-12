//*****************************************************************************
// 2 pole Butterworth filter
//*****************************************************************************

#ifndef __FLT_H
#define __FLT_H

#include "mdl.h"

NS_SSN1K_BEGIN

#define FLT_CUTOFF_MOD_RANGE 4000

enum SSN1K_FILTER_MODE : BYTE {
	SSN1K_FM_LP = 1,
	SSN1K_FM_HP = 2
};

struct FltCtrls : public MdlCtrls {
	Ctrl res;
	Ctrl mode;
};

class Flt : public Mdl {
private:
	float ai[3];
	float bi[3];
	float ci[3];
	float ui[2];
	float vi[2];
	float lp[2];
	float hp[2];
public:
	Flt();
	float run(float in, float cut);
};

#endif

NS_SSN1K_END