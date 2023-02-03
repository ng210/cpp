#ifndef __SYNTH_ADAPTER_H
#define __SYNTH_ADAPTER_H

#include "player/src/adapter.h"

NS_FW_BASE_USE
using namespace PLAYER;
namespace SYNTH {

	typedef enum SynthDevices {
		DeviceSynth,
		DeviceBass,
		DeviceMixer,
		DeviceDelay,
		DeviceClip
	} SynthDevices;

	class SynthAdapter : public Adapter {
		static AdapterInfo adapterInfo_;
	public:
		SynthAdapter();
		~SynthAdapter();

		float samplingRate;

		AdapterInfo* const getInfo();
		void initialize(byte** pData);
		Device* createDevice(int deviceType);
#ifdef PLAYER_EDIT_MODE
		void writeToStream(Stream* data);
#endif
	};
}

#endif