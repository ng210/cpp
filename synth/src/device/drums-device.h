#ifndef __DRUMS_DEVICE_H
#define __DRUMS_DEVICE_H

#include "synth/src/device/synth-adapter.h"
#include "synth/src/device/module-device.h"
#include "synth/src/module/drums.h"


using namespace PLAYER;
namespace SYNTH {

	class DrumsDevice : public ModuleDevice {
	public:
		DrumsDevice(SynthAdapter* adapter);
		virtual ~DrumsDevice();

		//void initialize(byte** pData = NULL);
		void processCommand(byte cmd, byte*& cursor);

		Drums* drums() { return (Drums*)object_; };

		Sequence* createDefaultSequence();
	};
}
#endif