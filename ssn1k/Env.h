//*****************************************************************************
// ADSR envelope
//*****************************************************************************

#ifndef __ENV_H
#define __ENV_H

#define _USE_LINEAR_CURVES

#include "Mdl.h"

#define OVERLAY_LENGTH 48

NS_SSN1K_BEGIN

enum SSN1K_ENV_PHASE {
	 SSN1K_ENV_IDLE
	,SSN1K_ENV_ATTACK
	,SSN1K_ENV_DECAY
	,SSN1K_ENV_SUSTAIN
	,SSN1K_ENV_RELEASE
};

struct EnvCtrls : public MdlCtrls {
	Ctrl atk;
	Ctrl dec;
	Ctrl sus;
	Ctrl rel;
	Ctrl gate;
};

class Env : public Mdl {
	float timer_;
	int phase_;
	//float fDelta;
	float velocity_;
public:
	Env(void);
	int overlayCounter_;
	int isActive();
	float run(float);
	void slopeUp();
	void slopeDown();
};

NS_SSN1K_END

#endif