#ifndef __MIX_H
#define __MIX_H

#include "Mdl.h"
#include "Synth.h"

NS_SSN1K_BEGIN

struct MixCtrls : public MdlCtrls {
	Ctrl* volume[16];
	Ctrl* balance[16];
};

enum MixerControlIds {
	SSN1K_CI_MixMix,
	SSN1K_CI_MixAmp,
	SSN1K_CI_MixBal,

	SSN1K_CI_MixVolume1,
	SSN1K_CI_MixVolume2,
	SSN1K_CI_MixVolume3,
	SSN1K_CI_MixVolume4,
	SSN1K_CI_MixVolume5,
	SSN1K_CI_MixVolume6,
	SSN1K_CI_MixVolume7,
	SSN1K_CI_MixVolume8,
	SSN1K_CI_MixVolume9,
	SSN1K_CI_MixVolume10,
	SSN1K_CI_MixVolume11,
	SSN1K_CI_MixVolume12,
	SSN1K_CI_MixVolume13,
	SSN1K_CI_MixVolume14,
	SSN1K_CI_MixVolume15,
	SSN1K_CI_MixVolume16,

	SSN1K_CI_MixBalance1,
	SSN1K_CI_MixBalance2,
	SSN1K_CI_MixBalance3,
	SSN1K_CI_MixBalance4,
	SSN1K_CI_MixBalance5,
	SSN1K_CI_MixBalance6,
	SSN1K_CI_MixBalance7,
	SSN1K_CI_MixBalance8,
	SSN1K_CI_MixBalance9,
	SSN1K_CI_MixBalance10,
	SSN1K_CI_MixBalance11,
	SSN1K_CI_MixBalance12,
	SSN1K_CI_MixBalance13,
	SSN1K_CI_MixBalance14,
	SSN1K_CI_MixBalance15,
	SSN1K_CI_MixBalance16,

	SSN1K_CI_MIX_COUNT
};

class Mixer : public Mdl {
	int inputCount_;
	Synth* input_[16];

	void writeSample(short* ptr, float smp);
public:
	Mixer();
	virtual ~Mixer();

	void addInput(Synth* in);

	float run(void* buffer, size_t start, size_t end);
};

NS_SSN1K_END

#endif