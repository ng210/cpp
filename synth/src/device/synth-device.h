#ifndef __SYNTH_DEVICE_H
#define __SYNTH_DEVICE_H

#include "synth/src/device/synth-adapter.h"
#include "synth/src/device/synth-base-device.h"
#include "synth/src/module/synth.h"
#include "synth/src/module/synth2.h"

using namespace PLAYER;
namespace SYNTH {

	class SynthDevice : public SynthBaseDevice
	{
	//protected: PROP(int, samplingRate);
	//protected: PROP(int, voiceCount);
	protected:
	public:
		SynthDevice(SynthAdapter* adapter);
		~SynthDevice();
		void initialize(int voiceCount);
#pragma region Device
		//void initialize(byte** pData = NULL);

		//bool isActive();
		//void isActive(bool b);
		//int run(int ticks);
		//void setRefreshRate(float fps);
		//void processCommand(byte cmd, byte*& cursor);

		SynthBase* synth();

		Sequence* createDefaultSequence();
#pragma endregion

//#ifdef PLAYER_EDIT_MODE
//		void makeCommandImpl(int command, byte*& stream, va_list args);
//		int getCommandSize(byte cmd, byte* args);
//		int writeToStream(Stream* stream);
//#endif
//
//#pragma region Synth
//		void setNote(byte note, byte velocity);
//		void setProgram(byte prgId);
//		byte* soundBank();
//		void soundBank(byte* data);
//		//void run(short* buffer, int start, int end);
//#pragma endregion
	};
}
#endif