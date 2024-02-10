#include <stdarg.h>
#include "base/memory.h"
#include "player.h"
#include "channel.h"
#include "sequence.h"

#include "player-adapter.h"

NS_FW_BASE_USE

namespace PLAYER {

	#pragma region Creation
	PlayerDevice* PlayerDevice::create(byte** pData) {
		// ensure player adapter exists
		var pa = NEW_(PlayerAdapter);
		var adapter = Player::getAdapter(pa->getInfo()->id);
		if (adapter == NULL) {
			Player::addAdapter(pa);
			adapter = pa;
		} else {
			DEL_(pa);
		}
		var device = (PlayerDevice*)adapter->createDevice(PlayerDevices::DevicePlayer, NULL);
		device->initialize(pData);
		device->type(PlayerDevices::DevicePlayer);
		return device;
	}

	PlayerDevice::PlayerDevice(Adapter* adapter, void* object) : Device(adapter, object) {
		mode_ = PlayerModeSequence;
		type_ = PlayerDevices::DevicePlayer;
		player_ = (Player*)object;
	}

	PlayerDevice::~PlayerDevice() {
		DEL_((Player*)object_);
	}

	void PlayerDevice::initialize(byte** pData) {
		player()->masterDevice(this);
		player()->initialize(pData);
	}
	#pragma endregion

	#pragma region Device implementation
	bool PlayerDevice::isActive() {
		return ((Player*)object())->masterChannel()->isActive();
	}
	void PlayerDevice::isActive(bool b) {
		((Player*)object())->masterChannel()->isActive(b);
	}

	void PlayerDevice::setValue(int id, Value value) {

	}

	Value* PlayerDevice::getValue(int id) {
		return NULL;
	}

	int PlayerDevice::run(int ticks) {
		return 0;
	}

	//void PlayerDevice::setRefreshRate(float fps) {
	//	var player = (Player*)object();
	//	for (int i = 1; i < player->devices().length(); i++) {
	//		((Device*)player->devices().get(i))->setRefreshRate(fps);
	//	}
	//}

	void PlayerDevice::processCommand(byte cmd, byte*& cursor) {
		var player = (Player*)object_;
		switch (cmd) {
			case PlayerCommands::CmdAssign:
				int channelId = READ(cursor, byte);
				int sourceId = READ(cursor, byte);
				int deviceId = READ(cursor, byte);
				int loopCount = READ(cursor, byte);
				var targetChannel = (Channel*)player->channels().get(channelId);
				var device = (Device*)player->devices().get(deviceId);
				if (mode_ == PlayerModeSequence) {
					var sequence = (Sequence*)player->sequences().get(sourceId);
					targetChannel->assign(device, sequence, loopCount);
				}
				else {
					var frames = (FrameList*)player->frameLists().get(sourceId);
					targetChannel->assign(device, frames, loopCount);
				}
				break;
		}
	}
	
	#pragma endregion
}