#ifndef __DISTORT_DEVICE_H
#define __DISTORT_DEVICE_H

#include "synth/src/device/synth-adapter.h"
#include "synth/src/device/module-device.h"
#include "synth/src/module/distort.h"

using namespace PLAYER;
namespace SYNTH {

	typedef struct DistortInputs {
		ClpInputs clp;
		FltInputs flt;
	} DistortInputs;
	#define DistortInputSize (ClpInputSize + FltInputSize)

	typedef enum DistortInputIds {
		distAmp,
		distLvl,
		distCut,
		distRes,
		distMode
	} DistortInputIds;

	class DistortDevice: public ModuleDevice {
	protected: DistortInputs distortInputs;
	public:
		DistortDevice(SynthAdapter* adapter, Player* player);
		~DistortDevice();

		int getPresetBankSize();
		PresetBank* getDefaultPresetBank();
	};
}
#endif