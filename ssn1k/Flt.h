//*****************************************************************************
// 2 pole Butterworth filter
//*****************************************************************************

#ifndef __FLT_H
#define __FLT_H

#include "Mdl.h"

NS_SSN1K_BEGIN

#define FLT_CUTOFF_MOD_RANGE 4000

enum SSN1K_FILTER_MODE {
	SSN1K_FM_LP = 1,
	SSN1K_FM_HP = 2
};

struct FltCtrls : public MdlCtrls {
	Ctrl* res;
	Ctrl* mode;
};

class Flt : public Mdl{
	float ai[3];
	float bi[3];
	float ci[3];
	float ui[2];
	float vi[2];
	float lp[2];
	float hp[2];
protected: PROP_R(Ctrl*, res);
protected: PROP_R(Ctrl*, mode);
public:
	Flt();
	float run(float in, float FCutMod);
	void setControls(FltCtrls*);
};

#endif

NS_SSN1K_END