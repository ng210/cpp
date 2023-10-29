#ifndef __SYNTH2_DEVICE_H
#define __SYNTH2_DEVICE_H

#include "synth/src/device/synth-adapter.h"
#include "synth/src/device/synth-base-device.h"
#include "synth/src/module/synth2.h"

using namespace PLAYER;
namespace SYNTH {

	class Synth2Device : public SynthBaseDevice
	{
	protected:
	public:
		Synth2Device(SynthAdapter* adapter);
		~Synth2Device();
		void initialize(int voiceCount);

		SynthBase* synth();

		Sequence* createDefaultSequence();
	};
}
#endif