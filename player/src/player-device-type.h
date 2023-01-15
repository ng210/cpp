#ifndef __PLAYER_PLAYER_DEVICE_TYPE_H
#define __PLAYER_PLAYER_DEVICE_TYPE_H

#include "device-type.h"

NS_FW_BASE_USE

namespace PLAYER {

	typedef enum PlayerDevices {
		DevicePlayer = 1,
		DeviceChannel = 2
	} PlayerDevices;

	class PlayerDeviceType : public DeviceType {
	public:
		static const int TypeId;
		static char* const TypeName;

		PlayerDeviceType();
		virtual ~PlayerDeviceType();

		void prepareContext(byte** pData);
		Device* createDeviceImpl(int type);

		int writeInitDataToStream(Stream* stream);
	};
}

#endif