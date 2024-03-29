#ifndef __SYNTH_ADAPTER_H
#define __SYNTH_ADAPTER_H

#include "player/src/adapter.h"

NS_FW_BASE_USE
using namespace PLAYER;
namespace SYNTH {

	typedef enum SynthDevices {
		DeviceSynth,
		DeviceBass,
		DeviceGenericDrum,
		DeviceDrums,
		DeviceMixer,
		DeviceStereoDelay,
		DeviceDistort,
		DeviceSynth2
	} SynthDevices;

	class SynthAdapter : public Adapter {
		static AdapterInfo adapterInfo_;
	public:
		SynthAdapter();
		~SynthAdapter();

		float samplingRate;

		AdapterInfo* const getInfo();
		void initialize(byte** pData);
		void prepare();
		void cleanUp();
		PArray* getDevices();
		Device* createDevice(int deviceType, Player* player);
#ifdef PLAYER_EDIT_MODE
		void writeToStream(Stream* data);
#endif
	};
}

#endif