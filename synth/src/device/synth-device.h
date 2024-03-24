#ifndef __SYNTH_DEVICE_H
#define __SYNTH_DEVICE_H

#include "synth/src/device/synth-adapter.h"
#include "synth/src/device/synth-base-device.h"
#include "synth/src/module/synth.h"

using namespace PLAYER;
namespace SYNTH {

	typedef struct SynthInputs {
		AdsrInputs amAdsr, fmAdsr, pmAdsr, ftAdsr;
		LfoInputs lfo1, lfo2;
		OscInputs osc1, osc2;
		FltInputs flt1;
	} SynthInputs;

	typedef enum SynthInputIds {
		// envelopes
		SynthInputAmEnvAmp, SynthInputAmEnvAtk, SynthInputAmEnvDec, SynthInputAmEnvSus, SynthInputAmEnvRel,
		SynthInputFmEnvAmp, SynthInputFmEnvAtk, SynthInputFmEnvDec, SynthInputFmEnvSus, SynthInputFmEnvRel,
		SynthInputPmEnvAmp, SynthInputPmEnvAtk, SynthInputPmEnvDec, SynthInputPmEnvSus, SynthInputPmEnvRel,
		SynthInputFtEnvAmp, SynthInputFtEnvAtk, SynthInputFtEnvDec, SynthInputFtEnvSus, SynthInputFtEnvRel,
		// LFOs
		SynthInputAmLfoAmp, SynthInputAmLfoFre,
		SynthInputFmLfoAmp, SynthInputFmLfoFre,
		// oscillators
		SynthInputOsc1Amp, SynthInputOsc1Fre, SynthInputOsc1Note, SynthInputOsc1Tune, SynthInputOsc1Psw, SynthInputOsc1Wave,
		SynthInputOsc2Amp, SynthInputOsc2Fre, SynthInputOsc2Note, SynthInputOsc2Tune, SynthInputOsc2Psw, SynthInputOsc2Wave,
		// filter
		SynthInputFlt1Cut, SynthInputFlt1Res, SynthInputFlt1Mode,
	} SynthInputIds;

#define SynthInputSize (4*AdsrInputsSize + 2*LfoInputsSize + 2*OscInputsSize + FltInputsSize)

	class SynthDevice : public SynthBaseDevice
	{
	//protected: PROP(int, samplingRate);
	//protected: PROP(int, voiceCount);
	protected:
	public:
		SynthInputs synthInputs;

		SynthDevice(SynthAdapter* adapter, Player* player);
		~SynthDevice();

		int getPresetBankSize();
		PresetBank* getDefaultPresetBank();

		SynthBase* synth();
	};
}
#endif