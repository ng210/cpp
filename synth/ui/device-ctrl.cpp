#include "synth/ui/device-ctrl.h"

using namespace PLAYER;

DeviceCtrl::DeviceCtrl(Device* device) : device_(device) {

}

DeviceCtrl::~DeviceCtrl() {

}

void DeviceCtrl::create(Window* parent, char* name) {
	Ctrl::create(DeviceCtrl::wndClass_, parent, name);
}
