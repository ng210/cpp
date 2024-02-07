#ifndef __PLAYER_PLAYER_EXT_H
#define __PLAYER_PLAYER_EXT_H

#include "base/stream.h"
#include "player/src/player.h"
#include "player/src/device-ext.h"

NS_FW_BASE_USE

namespace PLAYER {

	typedef DeviceExt* (DEVICEEXTENSIONCREATOR)(Device*);

	class PlayerExt {
		static Map* deviceExtensions_;
	protected: PROP_R(Player*, player);
	public:
		PlayerExt(Player*);
		//void load(byte** pData);
		Stream* save();

		DeviceExt* getDeviceExtension(Device* device);

		static void addDeviceExtension(int key, DEVICEEXTENSIONCREATOR* deviceExtCreator);
		static void cleanUp();
	};
}

#endif