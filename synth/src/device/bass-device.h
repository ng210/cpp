#ifndef __BASS_DEVICE_H
#define __BASS_DEVICE_H

#include "synth/src/device/synth-adapter.h"
#include "synth/src/module/bass.h"
#include "synth/src/device/synth-base-device.h"

using namespace PLAYER;
namespace SYNTH {

	typedef struct BassInputs {
		AdsrInputs amAdsr, pmAdsr, ftAdsr;
		OscInputs osc1, osc2;
		FltInputs flt1;
	} BassInputs;
	#define BassInputSize (3*AdsrInputSize + 2*OscInputSize + FltInputSize)
	typedef enum BassInputIds {
		BassInputAmAmp, BassInputAmAtk, BassInputAmDec, BassInputAmSus, BassInputAmRel,
		BassInputPmAmp, BassInputPmAtk, BassInputPmDec, BassInputPmSus, BassInputPmRel,
		BassInputFtAmp, BassInputFtAtk, BassInputFtDec, BassInputFtSus, BassInputFtRel,
		BassInputOsc1Amp, BassInputOsc1Fre, BassInputOsc1Note, BassInputOsc1Tune, BassInputOsc1Psw, BassInputOsc1Wave,
		BassInputOsc2Amp, BassInputOsc2Fre, BassInputOsc2Note, BassInputOsc2Tune, BassInputOsc2Psw, BassInputOsc2Wave,
		BassInputFlt1Cut, BassInputFlt1Res, BassInputFlt1Mode
	} BassInputIds;

	class BassDevice : public SynthBaseDevice
	{
		//protected: PROP(int, samplingRate);
		//protected: PROP(int, voiceCount);
		//PROP_R(BassInputs, bassInputs);
	protected:
	public:
		BassInputs bassInputs;

		BassDevice(SynthAdapter* adapter, Player* player);
		~BassDevice();

		int getPresetBankSize();
		PresetBank* getDefaultPresetBank();
	};
}
#endif