#ifndef __SYNTH_ADAPTER_H
#define __SYNTH_ADAPTER_H

#include "player/src/adapter.h"

NS_FW_BASE_USE
using namespace PLAYER;
namespace SYNTH {

	typedef enum SynthCommands {
		CmdSetNote = 2,
		CmdSetUint8,
		CmdSetFloat8,
		CmdSetFloat,
		CmdSetVelocity,
		CmdSetProgram
	} SynthCommands;

	typedef enum SynthDevices {
		DevSynth,
		DevDelay
	} SynthDevices;

	class SynthAdapter : Adapter {
	protected: PROP_R(int, samplingRate);
	protected: PROP_R(Player*, player);
	protected: PROP(int, frame);
	protected: PROP(float, samplePerFrame);
	public:
		SynthAdapter(Player* player);
		~SynthAdapter();

		void setRefreshRate(float fps);
		AdapterInfo* info();
		void prepareContext(byte* data);
		Device* createDeviceImpl(int deviceType, byte** initData);
		byte* processCommand(Channel* channel, byte command);

		Stream* makeCommand(byte command, ...);
		int getCommandArgsSize(byte command, byte* stream);

		static AdapterInfo Info;
		static void fillSoundBuffer(short* buffer, int bufferSize, void* args);
		static Adapter* creator(Player* player);
		static void initializer(void* args);
	};
}

#endif