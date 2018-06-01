//*****************************************************************************
// ADSR envelope
//*****************************************************************************

#ifndef __ENV_H
#define __ENV_H

#define _USE_LINEAR_CURVES

#include "mdl.h"
#include "basedef.h"

//#define OVERLAY_LENGTH 48

NS_SSN1K_BEGIN

enum SSN1K_ENV_PHASE : UINT8 {
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
};

class Env : public Mdl {
private: 
	float timer_;
	int phase_;
	int gate_;
	//float fDelta;
	float rate_;
	float velocity_;
	float* ticksPerSample_;
public:
	Env(void);
	//int overlayCounter_;
	int isActive();
	void reset();
	float run();
	void setGate(float velocity);
	void ticksPerSample(float* v);
};

NS_SSN1K_END

#endif