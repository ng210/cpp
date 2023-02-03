#ifndef __BASS_DEVICE_H
#define __BASS_DEVICE_H

#include "synth/src/device/synth-adapter.h"
#include "synth/src/device/synth-base-device.h"
#include "synth/src/module/bass.h"

using namespace PLAYER;
namespace SYNTH {

	class BassDevice : public SynthBaseDevice
	{
	//protected: PROP(int, samplingRate);
	//protected: PROP(int, voiceCount);
	protected:
	public:
		BassDevice(SynthAdapter* adapter);
		virtual ~BassDevice();
	};
}
#endif