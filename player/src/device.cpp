#include "device.h"
#include <stdarg.h>
#include "base/memory.h"
#include "player/src/device.h"

NS_FW_BASE_USE

namespace PLAYER {

	Device::Device(void* obj, Adapter* ad) {
		setRefreshRate(24.0f);
		object(obj);
		type(0);
		isActive(false);
		adapter(ad);
		initData_ = NULL;
	}

	//void Device::dataBlockItem(DataBlockItem* db) {
	//	dataBlockItem_.dataBlock = db->dataBlock;
	//	dataBlockItem_.length = db->length;
	//	dataBlockItem_.flag = db->flag;
	//}

	bool Device::isActive() {
		return isActive_;
	}
	void Device::isActive(bool b) {
		isActive_ = b;
	}

	int Device::run(int ticks) {
		return 0;
	}
	void Device::setRefreshRate(float fps) {
		refreshRate_ = fps;
	}

#ifdef PLAYER_EDIT_MODE
	//byte* Device::makeCommand(byte command, ...) {
	//	va_list args;
	//	va_start(args, command);
	//	byte* bytes = (byte*)MALLOC(byte, getCommandSize(command, (byte*)args));
	//	var p = bytes;
	//	*p++ = command;
	//	makeCommandImpl(command, p, args);
	//	va_end(args);
	//	return bytes;
	//}

	Stream* Device::makeCommand(byte command, ...) {
		va_list args;
		va_start(args, command);
		var stream = NEW_(Stream, getCommandSize(command, (byte*)args));
		stream->writeByte(command);
		makeCommandImpl(command, stream, args);
		va_end(args);
		return stream;
	}

	int Device::writeToStream(Stream* stream) {
		stream->writeByte(type_);
		return 1;
	}
#endif
}