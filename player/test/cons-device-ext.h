#ifndef __CONS_DEVICE_EXT_H
#define __CONS_DEVICE_EXT_H

#include <stdarg.h>
#include "player/src/player-ext.h"
#include "player/src/device-ext.h"
#include "player/test/cons-device.h"

NS_FW_BASE_USE

namespace PLAYER {

    class ConsDeviceExt : public DeviceExt {
		static DEVICEEXTENSIONCREATOR consDeviceExtCreator_;
	public:
		ConsDeviceExt(Device* device);
		~ConsDeviceExt();
		void setupInputs();
		InputBase* getInput(int id);
		void makeCommandImpl(int command, Stream* stream, va_list args);
		int getCommandSize(byte* cmd);
		Sequence* createDefaultSequence();
		void writeToStream(Stream* stream);
		static void registerExtensionCreator();
    };
}
#endif