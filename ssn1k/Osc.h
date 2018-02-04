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
private:
	float timer_;
	float noiseTimer_;
public:
	Osc(void);
	float run(float amp, float tune, float freq, float phase);
};

NS_SSN1K_END

#endif