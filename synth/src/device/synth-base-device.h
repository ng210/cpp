#ifndef __SYNTH_BASE_DEVICE_H
#define __SYNTH_BASE_DEVICE_H

#include "synth/src/device/synth-adapter.h"
#include "synth/src/device/module-device.h"
#include "synth/src/module/synth-base.h"

using namespace PLAYER;
namespace SYNTH {

	class SynthBaseDevice : public ModuleDevice
	{
		//protected: PROP(int, samplingRate);
		//protected: PROP(int, voiceCount);
	protected:
	public:
		SynthBaseDevice(SynthAdapter* adapter, Player* player, void* object);
		~SynthBaseDevice();

		void initialize(byte** pData = NULL);
		bool isActive();
		//void isActive(bool b);
		void processCommand(byte cmd, byte*& cursor);

		inline SynthBase* synthBase() { return (SynthBase*)object_; };

		void setNote(byte note, byte velocity);
	};
}
#endif