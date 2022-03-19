#include "adapter.h"
#include "device.h"
#include <base/memory.h>

using namespace PLAYER;

Adapter::Adapter() {
    player_ = NULL;
    devices_.init(8);
}

Adapter::Adapter(Player* player) {
    player_ = player;
    devices_.init(8);
}

Adapter::~Adapter() {
    devices_.forEach([](void* device, UINT32 ix, Collection* arr, void* args) {
        DEL_((Device*)device);
        return 1;
    });
}

void Adapter::prepareContext(byte* data) {
    if (data) {
        var deviceCount = READ(data, byte);
        for (var i = 0; i < deviceCount; i++) {
            var devType = READ(data, byte);
            var device = createDevice(devType, &data);
        }
    }
}

Device* Adapter::createDevice(int deviceType, byte** initData) {
    var device = createDeviceImpl(deviceType, initData);
    if (device != NULL) {
        device->type(deviceType);
        devices_.add(device);
    }
    return device;
}

void Adapter::setRefreshRate(float fps) {
    devices_.forEach([](void* device, UINT32 ix, Collection* collection, void* args) {
            ((Device*)device)->setRefreshRate(*(float*)args);
            return 1;
        }, &fps);
}