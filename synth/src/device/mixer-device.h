#ifndef __MIXER_DEVICE_H
#define __MIXER_DEVICE_H

#include "player/src/device.h"
#include "collection/parray.h"
#include "synth-adapter.h"
#include "module-device.h"
#include "../module/mixer.h"

NS_FW_BASE_USE
using namespace PLAYER;
namespace SYNTH {

	typedef enum MixerCommands {
	} MixerCommands;

	class Mixer8x4;
	class MixerDevice : public ModuleDevice {
	protected: PROP(int, frame);
	protected: PROP(float, samplePerFrame);

	public:
		MixerDevice(SynthAdapter* adapter);
		virtual ~MixerDevice();

#pragma region Device
		void initialize(byte** pData = NULL);
#pragma endregion

#ifdef PLAYER_EDIT_MODE
		//void makeCommandImpl(int command, Stream* stream, va_list args);
		//int getCommandSize(byte cmd, byte* args);
		int writeToStream(Stream* stream);
#endif
	};
}
#endif