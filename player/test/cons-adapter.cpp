#include <windows.h>
#include "base/memory.h"
#include "player/test/cons-adapter.h"
#include "player/test/cons-device.h"
#include "player/test/cons-device-ext.h"

using namespace PLAYER;

ConsoleCreator ConsAdapter::createConsole = { NULL, NULL };

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
Device* ConsAdapter::createDevice(int deviceType, Player* player) {
	Device* device = NULL;
	switch (deviceType) {
	case ConsDevices::DeviceCons:
		device = NEW_(ConsDevice, this, ConsAdapter::createConsole());
		break;
	}
	device->player(player);
	return device;
}

void ConsAdapter::writeToStream(Stream* data) {
	data->writeByte(getInfo()->id);
	data->writeDword(initData_);
}
