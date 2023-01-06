#ifndef __PLAYER_PLAYER_ADAPTER_H
#define __PLAYER_PLAYER_ADAPTER_H

#include "adapter.h"
#include "player-device.h"

NS_FW_BASE_USE

namespace PLAYER {

	class Player;
	class Channel;

	typedef enum PlayerDevices {
		DevicePlayer = 1
	} PlayerDevices;

	class PlayerAdapter : public Adapter {
		static AdapterInfo adapterInfo_;
		//PROP_R(Player*, player);
	public:
		//PlayerAdapter();
		//virtual ~PlayerAdapter();

		AdapterInfo* const getInfo();
		Device* createDevice(int deviceType);
		//void prepareContext(byte** pData);
	};
}

#endif