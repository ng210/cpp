#ifndef __PLAYER_PLAYER_DEVICE_EXT_H
#define __PLAYER_PLAYER_DEVICE_EXT_H

#include <stdarg.h>
#include "player/src/device-ext.h"
#include "player/src/player-ext.h"

NS_FW_BASE_USE

namespace PLAYER {

	#pragma
    class PlayerDeviceExt : public DeviceExt {
		static DEVICEEXTENSIONCREATOR playerDeviceExtCreator_;
	public:
		PlayerDeviceExt(Device* device);
		Player* player();
		void makeCommandImpl(int command, Stream* stream, va_list args);
		int getCommandSize(byte* cmd);

		static void registerExtensionCreator();
    };

}
#endif