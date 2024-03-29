#include "player/src/device.h"

NS_FW_BASE_USE

using namespace PLAYER;

Device::Device() : Device(NULL, NULL) {
}

Device::Device(Adapter* adapter, Player* player, void* obj) {
	adapter_ = adapter;
	player_ = player;
	object(obj);
	type(0);
	isActive(false);
	inputCount(0);
	presetBank_ = NULL;
	preset_ = 0;
	//setRefreshRate(24.0f);
	//adapter(ad);
	//initData_ = NULL;
	setPresetBank.add(this, Device::presetBankSetter);
	setPreset.add(this, Device::presetSetter);
}

bool Device::isActive() {
	return isActive_;
}
void Device::isActive(bool b) {
	isActive_ = b;
}

InputBase* Device::getInput(int id) {
	return &inputs_[id];
}

void Device::setInput(int id, Value v) {
	inputs_[id].set(v);
}

//Value* Device::getValue(int id) {
//	return getInput(id)->value();
//}

int Device::run(int ticks) {
	return 0;
}

//void Device::setRefreshRate(float fps) {
//	refreshRate_ = fps;
//}

int Device::presetBankSetter(void* obj, PresetBank* presetBank, void* unused) {
	var dev = (Device*)obj;
	var pb = dev->presetBank_;
	if (presetBank != NULL) {
		dev->presetBank_ = presetBank;
		dev->setPreset(0);
	}
	return pb == NULL;
}

int Device::presetSetter(void* obj, int ix, void* unused) {
	var dev = (Device*)obj;
	dev->preset_ = ix;
	byte* pb = NULL;
	if (dev->presetBank_) {
		pb = dev->presetBank_->getPreset(ix);
		if (pb != NULL) {
			for (var ii = 0; ii < dev->inputCount_; ii++) {
				dev->getInput(ii)->readValueFromStream(pb);
			}
		}
	}
	return pb == NULL;
}
