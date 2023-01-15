#include "base/memory.h"
#include "collection/map.h"
#include "player.h"
#include "player-device-type.h"
#include "player-device.h"
#include "channel.h"

NS_FW_BASE_USE

namespace PLAYER {

	const int PlayerDeviceType::TypeId = 1;
	char* const PlayerDeviceType::TypeName = "Player";

	PlayerDeviceType::PlayerDeviceType() : DeviceType(PlayerDeviceType::TypeId, PlayerDeviceType::TypeName) {
	}

	PlayerDeviceType::~PlayerDeviceType() {

	}

	void PlayerDeviceType::prepareContext(byte** pData) {
		var fps = *(float*)*pData;
		*pData += sizeof(float);
		parent_->device()->setRefreshRate(fps);
	}

	//void PlayerDeviceType::prepareContext(byte** data) {

	//}

	Device* PlayerDeviceType::createDeviceImpl(int type) {
		Device* device = NULL;
		switch (type) {
		case PlayerDevices::DevicePlayer:
			device = NEW_(PlayerDevice);
			break;
		case DeviceChannel:
			device = NEW_(Channel);
			parent_->device()->channels().add(device);
			break;
		}
		return device;
	}

	int PlayerDeviceType::writeInitDataToStream(Stream* stream) {
		stream->writeFloat(parent_->device()->refreshRate());
		return sizeof(parent_->device()->refreshRate());
	}
}
