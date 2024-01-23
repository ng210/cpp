#include "player/src/device.h"

NS_FW_BASE_USE

namespace PLAYER {

	Device::Device() : Device(NULL, NULL) {

	}

	Device::Device(Adapter* adapter, void* obj) {
		adapter_ = adapter;
		object(obj);
		type(0);
		isActive(false);
		inputCount(0);
		presetBank_ = NULL;
		preset_ = -1;
		//setRefreshRate(24.0f);
		//adapter(ad);
		//initData_ = NULL;
	}

	bool Device::isActive() {
		return isActive_;
	}
	void Device::isActive(bool b) {
		isActive_ = b;
	}

	int Device::run(int ticks) {
		return 0;
	}

	//void Device::setRefreshRate(float fps) {
	//	refreshRate_ = fps;
	//}

	PresetBank* Device::setPresetBank(PresetBank* presetBank) {
		var pb = presetBank_;
		presetBank_ = presetBank;
		return pb;
	}

	int Device::setPreset(int presetId) {
		preset_ = presetId;
		return 0;
	}
}