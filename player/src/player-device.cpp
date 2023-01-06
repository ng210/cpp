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
		var adapter = NEW_(PlayerAdapter);
		Player::addAdapter(adapter);
		var device = (PlayerDevice*)adapter->createDevice(PlayerDevices::DevicePlayer);
		device->initialize(pData);
		//device->player()->devices().add(device);
		device->type(PlayerDevices::DevicePlayer);
		return device;
	}

	PlayerDevice::PlayerDevice(void* object, Adapter* adapter) : Device(object, adapter) {
		// 1st device is this player-device itself
		//devices_.add(this);
		//masterChannel_ = NEW_(Channel, "master");
		//channels_.add(masterChannel_);
		type_ = PlayerDevices::DevicePlayer;
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

	void PlayerDevice::setRefreshRate(float fps) {
		var player = (Player*)object();
		for (int i = 1; i < player->devices().length(); i++) {
			((Device*)player->devices().getAt(i))->setRefreshRate(fps);
		}
	}

	void PlayerDevice::processCommand(byte cmd, byte*& cursor) {
		var player = (Player*)object_;
		switch (cmd) {
			case PlayerCommands::CmdAssign:
				int channelId = READ(cursor, byte);
				int sequenceId = READ(cursor, byte);
				int deviceId = READ(cursor, byte);
				int loopCount = READ(cursor, byte);
				var targetChannel = (Channel*)player->channels().getAt(channelId);
				var device = (Device*)player->devices().getAt(deviceId);
				var sequence = (Sequence*)player->sequences().getAt(sequenceId);
				targetChannel->assign(device, sequence, loopCount);
				break;
		}

	}
	
#ifdef PLAYER_EDIT_MODE
	void PlayerDevice::makeCommandImpl(int command, byte*& stream, va_list args) {
		switch (command) {
		case PlayerCommands::CmdAssign:
			// byte chnId, byte seqId, byte devId, byte loopCount
			*stream++ = (byte)va_arg(args, int);
			*stream++ = (byte)va_arg(args, int);
			*stream++ = (byte)va_arg(args, int);
			*stream++ = (byte)va_arg(args, int);
			break;
		case PlayerCommands::CmdTempo:
			// float framePerSecond
			*(float*)stream = va_arg(args, float);
			stream += sizeof(float);
			break;
		case PlayerCommands::CmdEOF:
			// no arguments
			break;
		case PlayerCommands::CmdEOS:
			// no arguments
			break;
		}
	}

	int PlayerDevice::getCommandSize(byte cmd, byte* args) {
		var length = 1;
		switch (cmd) {
			case CmdAssign:
				// byte chnId, byte seqId, byte devId, byte loopCount
				length += 4;
				break;
			case CmdTempo:
				// float framePerSecond
				length += sizeof(float);
				break;
			case CmdEOF:
				break;
			case CmdEOS:
				break;
		}
		return length;
	}

	int PlayerDevice::writeToStream(Stream* stream) {
		return 0;
	}
#endif

#pragma endregion

}