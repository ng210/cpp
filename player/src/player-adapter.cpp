#include <stdio.h>
#include "base/memory.h"
#include "player.h"
#include "player-adapter.h"
#include "player-device.h"


NS_FW_BASE_USE
namespace PLAYER {

    AdapterInfo PlayerAdapter::adapterInfo_ = { "Player", 1 };

    AdapterInfo* const PlayerAdapter::getInfo() {
        return &PlayerAdapter::adapterInfo_;
    }

    Device* PlayerAdapter::createDevice(int deviceType) {
        Device* device = NULL;
        switch (deviceType) {
        case PlayerDevices::DevicePlayer:
            device = NEW_(PlayerDevice, this, NEW_(Player));
            break;
        }
        return device;
    }
}