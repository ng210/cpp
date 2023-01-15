#ifndef __SYNTH_DEVICE_TYPE_H
#define __SYNTH_DEVICE_TYPE_H

#include "player/src/device.h"
#include "player/src/device-type.h"
#include "synth.h"


using namespace PLAYER;
namespace SYNTH {

	typedef enum SynthDevices {
		DeviceSynth = 1,
		DeviceDelay = 2
	} SynthDevices;

	class SynthDeviceType : public DeviceType {
		PROP_R(int, samplingRate);
	public:		
		static const int TypeId;
		static char* const TypeName;

		SynthDeviceType();
		virtual ~SynthDeviceType();

		void prepareContext(byte** pData);
		Device* createDeviceImpl(int type);

		int writeInitDataToStream(Stream* stream);

		static void soundCallback(short* buffer, int length, void* context);
	};
}

#endif