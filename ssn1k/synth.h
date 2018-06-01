//*****************************************************************************
// Simple synth: in->osc->env->flt->out
//*****************************************************************************

#ifndef __SYNTH_H
#define __SYNTH_H

#include "mdl.h"
#include "osc.h"
#include "env.h"
#include "flt.h"

NS_SSN1K_BEGIN

struct SynthCtrls : public MdlCtrls {
	Ctrl tune;
	EnvCtrls env1, env2, env3, env4;
	OscCtrls osc1, osc2, lfo1, lfo2;
	FltCtrls flt;
};

enum SynthControlIds : UINT8 {
	 SSN1K_CI_SynthMix	// 0
	,SSN1K_CI_SynthAmp
	,SSN1K_CI_SynthBal
	,SSN1K_CI_Tune

	,SSN1K_CI_Env1Mix	// 3
	,SSN1K_CI_Env1Amp
	,SSN1K_CI_Env1DC
	,SSN1K_CI_Env1Atk
	,SSN1K_CI_Env1Dec
	,SSN1K_CI_Env1Sus
	,SSN1K_CI_Env1Rel

	,SSN1K_CI_Env2Mix	// 10
	,SSN1K_CI_Env2Amp
	,SSN1K_CI_Env2DC
	,SSN1K_CI_Env2Atk
	,SSN1K_CI_Env2Dec
	,SSN1K_CI_Env2Sus
	,SSN1K_CI_Env2Rel

	,SSN1K_CI_Env3Mix	// 17
	,SSN1K_CI_Env3Amp
	,SSN1K_CI_Env3DC
	,SSN1K_CI_Env3Atk
	,SSN1K_CI_Env3Dec
	,SSN1K_CI_Env3Sus
	,SSN1K_CI_Env3Rel

	,SSN1K_CI_Env4Mix	// 24
	,SSN1K_CI_Env4Amp
	,SSN1K_CI_Env4DC
	,SSN1K_CI_Env4Atk
	,SSN1K_CI_Env4Dec
	,SSN1K_CI_Env4Sus
	,SSN1K_CI_Env4Rel

	,SSN1K_CI_Osc1Mix	// 31
	,SSN1K_CI_Osc1Amp
	,SSN1K_CI_Osc1DC
	,SSN1K_CI_Osc1Note
	,SSN1K_CI_Osc1Tune
	,SSN1K_CI_Osc1Fre
	,SSN1K_CI_Osc1Psw
	,SSN1K_CI_Osc1Wav

	,SSN1K_CI_Osc2Mix	// 39
	,SSN1K_CI_Osc2Amp
	,SSN1K_CI_Osc2DC
	,SSN1K_CI_Osc2Note
	,SSN1K_CI_Osc2Tune
	,SSN1K_CI_Osc2Fre
	,SSN1K_CI_Osc2Psw
	,SSN1K_CI_Osc2Wav

	,SSN1K_CI_Lfo1Mix	// 47
	,SSN1K_CI_Lfo1Amp
	,SSN1K_CI_Lfo1DC
	,SSN1K_CI_Lfo1Note
	,SSN1K_CI_Lfo1Tune
	,SSN1K_CI_Lfo1Fre
	,SSN1K_CI_Lfo1Psw
	,SSN1K_CI_Lfo1Wav

	,SSN1K_CI_Lfo2Mix
	,SSN1K_CI_Lfo2Amp
	,SSN1K_CI_Lfo2DC
	,SSN1K_CI_Lfo2Note
	,SSN1K_CI_Lfo2Tune
	,SSN1K_CI_Lfo2Fre
	,SSN1K_CI_Lfo2Psw
	,SSN1K_CI_Lfo2Wav

	,SSN1K_CI_FltMix
	,SSN1K_CI_FltAmp
	,SSN1K_CI_FltDC
	,SSN1K_CI_FltRes
	,SSN1K_CI_FltMode

	,SSN1K_CI_COUNT
};

class Voice;
//*****************************************************************************
// Control: Ctrl*, Instrument: Ctrl**, Bank: Ctrl***
class Synth : public Mdl {
protected:
	//Array sweepControls;	//Array<Ctrl>
	//Ctrl **paSweepCtrls;
	UINT32 voiceCount_;
	Voice* voices_[64];
	UINT32 nextVoice_;
	PROP_R(Voice*, activeVoice);
	PROP_R(Ctrl**, bank);
	PROP_REF(float, ticksPerSample);
	//UINT32 bankCount_;
	UINT32 selectedBank_;
	//Ctrl** banks_[4];
	float overlayValue_;
	void init(UINT32 voices);
public:
	Synth(UINT32 voices = 64);
	~Synth();

#ifdef _PROFILE

	_Profiler profiler;

#endif
	float run();
	void run(void* buffer, UINT32 start, UINT32 end);
	void noteOn(int note, float velocity);
	void noteOff(int note);
	void changeProgram(int prgId);
	//void setCtrlSweep(int id, float fStart, float fEnd, int iCycles);
	void ticksPerSample(float bpm);
	void bank(Ctrl** bank);
	
	static void setControls(Ctrl* controls, UINT8* data);
};

NS_SSN1K_END

#endif