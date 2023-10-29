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
		SynthBaseDevice(SynthAdapter* adapter, void* object);
		~SynthBaseDevice();
#pragma region Device
		void initialize(byte** pData = NULL);

		bool isActive();
		//void isActive(bool b);

		int run(int ticks);
		void setRefreshRate(float fps);
		void processCommand(byte cmd, byte*& cursor);
#pragma endregion

		inline SynthBase* synthBase() { return (SynthBase*)object_; };

#ifdef PLAYER_EDIT_MODE
		int writeToStream(Stream* stream);
#endif

#pragma region Synth
		void setNote(byte note, byte velocity);
		//void run(short* buffer, int start, int end);
#pragma endregion
	};
}
#endif