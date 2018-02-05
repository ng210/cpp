//*****************************************************************************
// ADSR envelope
//*****************************************************************************

#ifndef __ENV_H
#define __ENV_H

#define _USE_LINEAR_CURVES

#include "Mdl.h"
#include "base/base_defs.h"

//#define OVERLAY_LENGTH 48

NS_SSN1K_BEGIN

enum SSN1K_ENV_PHASE {
	 SSN1K_ENV_IDLE
	,SSN1K_ENV_ATTACK
	,SSN1K_ENV_DECAY
	,SSN1K_ENV_SUSTAIN
	,SSN1K_ENV_RELEASE
};

struct EnvCtrls : public MdlCtrls {
	Ctrl* atk;
	Ctrl* dec;
	Ctrl* sus;
	Ctrl* rel;
};

class Env : public Mdl {
private: 
	float timer_;
	int phase_;
	int gate_;
	//float fDelta;
	float rate_;
	float velocity_;
	float tickPerSample_;
public:
	Env(void);

	PROP_R(float, bpm);
	//int overlayCounter_;
	int isActive();
	float run();
	void bpm(float bpm);
	void setGate(float velocity);
};

NS_SSN1K_END

#endif