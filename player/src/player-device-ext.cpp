#include "base/memory.h"
#include "player/src/player-ext.h"
#include "player/src/player-adapter.h"
#include "player/src/player-device-ext.h"

using namespace PLAYER;

DeviceExt* PlayerDeviceExt::playerDeviceExtCreator_(Device* device) {
	DeviceExt* deviceExt = NULL;
	switch (device->type()) {
		case PlayerDevices::DevicePlayer: deviceExt = NEW_(PlayerDeviceExt, device); break;
	}
	return deviceExt;
}

PlayerDeviceExt::PlayerDeviceExt(Device* device) : DeviceExt(device) {
}

Player* PlayerDeviceExt::player() {
	return ((PlayerDevice*)device_)->player();
}

void PlayerDeviceExt::makeCommandImpl(int command, Stream* stream, va_list args) {
	switch (command) {
	case PlayerCommands::CmdAssign:
		// byte chnId, byte seqId, byte devId, byte loopCount
		stream->writeByte((byte)va_arg(args, int));
		stream->writeByte((byte)va_arg(args, int));
		stream->writeByte((byte)va_arg(args, int));
		stream->writeByte((byte)va_arg(args, int));
		break;
	case PlayerCommands::CmdTempo:
		// float framePerSecond
		stream->writeFloat(va_arg(args, float));
		break;
	case PlayerCommands::CmdEOF:
		// no arguments
		break;
	case PlayerCommands::CmdEOS:
		// no arguments
		break;
	}
}

int PlayerDeviceExt::getCommandSize(byte* cmd) {
	var length = 1;
	switch (*cmd) {
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

void PlayerDeviceExt::registerExtensionCreator() {
	PlayerAdapter adapter;
	var key = (adapter.getInfo()->id << 8) + PlayerDevices::DevicePlayer;
	PlayerExt::addDeviceExtension(key, &PlayerDeviceExt::playerDeviceExtCreator_);
}