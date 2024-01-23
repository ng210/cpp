#ifndef __CONS_DEVICE_EXT_H
#define __CONS_DEVICE_EXT_H

#include <stdarg.h>
#include "player/src/player-ext.h"
#include "player/src/device-ext.h"
#include "player/test/cons-device.h"

NS_FW_BASE_USE

namespace PLAYER {

	typedef struct ConsDeviceInputs {
		Input x, y, ink;
	} ConsDeviceInputs;

    class ConsDeviceExt : public DeviceExt {
		static DEVICEEXTENSIONCREATOR consDeviceExtCreator_;
		ConsDeviceInputs consDeviceInputs_;
	public:
		ConsDeviceExt(Device* device);
		~ConsDeviceExt();

		Input* getInput(int id);
		void makeCommandImpl(int command, Stream* stream, va_list args);
		int getCommandSize(byte* cmd);
		Sequence* createDefaultSequence();

		static void registerExtensionCreator();
    };
}
#endif