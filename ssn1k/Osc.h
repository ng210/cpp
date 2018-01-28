//*****************************************************************************
// Multi waveform oscillator
//*****************************************************************************

#ifndef __OSC_H
#define __OSC_H

#include "Mdl.h"

NS_SSN1K_BEGIN

enum SSN1K_WaveForm {
	 SSN1K_WF_TRI	= 0x01
	,SSN1K_WF_PSAW	= 0x02
	,SSN1K_WF_PLS	= 0x04
	,SSN1K_WF_RND	= 0x08
	,SSN1K_WF_SIN	= 0x10

	,SSN1K_WF_COUNT
};

struct OscCtrls : public MdlCtrls {
	Ctrl* note;
	Ctrl* tune;
	Ctrl* fre;
	Ctrl* psw;
	Ctrl* wav;
};

class Osc : public Mdl {
	protected: PROP(Ctrl*, note);
	protected: PROP_R(Ctrl*, tune);
	protected: PROP_R(Ctrl*, fre);
	protected: PROP_R(Ctrl*, psw);
	protected: PROP_R(Ctrl*, wav);

	float timer_;
	float noiseTimer_;
public:
	Osc(void);
	float run(float am, float fm, float pm);
	void setControls(OscCtrls* controls);
};

NS_SSN1K_END

#endif