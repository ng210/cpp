#include "base/memory.h"
#include "cons-adapter.h"
#include "cons-device.h"

using namespace PLAYER;

AdapterInfo ConsAdapter::adapterInfo_ = { "Cons", 124 };

ConsAdapter::ConsAdapter() {
	initData_ = NULL;
}
ConsAdapter::~ConsAdapter() {

}

AdapterInfo* const ConsAdapter::getInfo() {
	return &ConsAdapter::adapterInfo_;
}
void ConsAdapter::initialize(byte** pData) {
	// read init data
	initData_ = READ(*pData, dword);

}
Device* ConsAdapter::createDevice(int deviceType) {
	Device* device = NULL;
	switch (deviceType) {
	case ConsDevices::DeviceCons:
		device = NEW_(ConsDevice, this);
		break;
	}
	return device;
}

void ConsAdapter::writeToStream(Stream* data) {
	data->writeByte(getInfo()->id);
	data->writeDword(initData_);
}
