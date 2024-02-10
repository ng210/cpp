#ifndef __BASS_DEVICE_H
#define __BASS_DEVICE_H

#include "synth/src/device/synth-adapter.h"
#include "synth/src/module/bass.h"
#include "synth/src/device/synth-base-device.h"

using namespace PLAYER;
namespace SYNTH {

	typedef struct BassInputs {
	public:
		AdsrInputs amAdsr, pmAdsr, ftAdsr;
		OscInputs osc1, osc2;
		FltInputs flt1;
	} BassInputs;

#define BassInputSize (3*AdsrInputsSize + 2*OscInputsSize + FltInputsSize)

	class BassDevice : public SynthBaseDevice
	{
	//protected: PROP(int, samplingRate);
	//protected: PROP(int, voiceCount);
		//PROP_R(BassInputs, bassInputs);
	protected:
	public:
		BassInputs bassInputs;

		BassDevice(SynthAdapter* adapter);
		virtual ~BassDevice();

		int getPresetBankSize();
		PresetBank* BassDevice::getDefaultPresetBank();
	};
}
#endif