#ifndef __SYNTH_DEVICE_H
#define __SYNTH_DEVICE_H


#include "player/src/device.h"
#include "synth.h"

using namespace PLAYER;
namespace SYNTH {
	;

	class SynthDevice : public Device
	{
	protected: PROP(Synth*, synth);
			 //protected: PROP(int, samplingRate);
			 //protected: PROP(int, voiceCount);
	public:
		SynthDevice(int samplingRate, int voiceCount);
		~SynthDevice();

		void setRefreshRate(float fps);

		void setNote(byte note, byte velocity);
		void setControl(byte ctrlId, byte value);
		void setControl(byte ctrlId, float value);
		void setProgram(byte prgId);
		byte* soundBank();
		void soundBank(byte* data);

		void run(short* buffer, int start, int end);
	};
}
#endif