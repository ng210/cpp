#include <stdarg.h>
#include "base/memory.h"
#include "player/src/device.h"
#include "player/src/player-adapter.h"

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
}