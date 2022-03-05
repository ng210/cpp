#include <stdio.h>
#include <stdarg.h>
#include "playeradapter.h"
#include "player.h"

NS_PLAYER_BEGIN

const ADAPTER_INFO PlayerAdapter::adapterInfo_ = {
	0x504C5900,		// PLY+00
	"PLY00"
};

PlayerAdapter PlayerAdapter::instance_;

void PlayerAdapter::createObjects(PArray* targets, void* data) {
	;
}

const ADAPTER_INFO& PlayerAdapter::getInfo() {
	return PlayerAdapter::adapterInfo_;
}

const IAdapter& PlayerAdapter::getInstance() {
	return PlayerAdapter::instance_;
}

void PlayerAdapter::prepareContext(void* data) {
	;
}
int PlayerAdapter::processCommand(void* playerObject, PLAYER_COMMAND_U data) {
	Player* player = (Player*)playerObject;
	PLAYER_COMMAND_ALL cmd;
	size_t ix = -1;
	ChannelBase* chn = NULL;
	cmd.base = data;
	switch (cmd.base.s->cmdId) {
		case Player_Cmd_setTempo:	// tpm
			float refreshRate;
			refreshRate = cmd.tempo->ticksPerMinute / 60.0f;
			ARRAY_FOREACH(player->channels(), ChannelBase* chn = ((ChannelBase*)value); chn->adapter().updateRefreshRate(chn->target(), refreshRate));
			break;
		case Player_Cmd_assign:		// target, adapter, sequence, status
			ChannelBase& chn = player->getFreeChannel();
			void* target = player->targets()->getAt(cmd.assign->target);
			SequenceBase* sequence = (SequenceBase*)player->sequences()->getAt(cmd.assign->sequence);
			IAdapter* adapter = (IAdapter*)player->adapters()->getAt(cmd.assign->adapter);
			chn.assign(target, *adapter, sequence, cmd.assign->status);
			break;
	}
	return 0;
}

void PlayerAdapter::updateRefreshRate(void* target, float ticksPerSecond) {
	((Player*)target)->refreshRate(ticksPerSecond);
}

//size_t PlayerAdapter::fill(void* buffer, size_t start, size_t end) {
//	return 0;
//}

#ifdef _EDITOR
	// editor extensions
PLAYER_COMMAND PlayerAdapter::createCommand(int code, ...) {
	PLAYER_COMMAND_ALL cmd;
	cmd.base = NULL;
	va_list args;
	va_start(args, code);
	switch (code) {
	case Player_Cmd_setTempo:
		cmd.tempo = MALLOC(PLAYER_CMD_TEMPO, 1);
		cmd.tempo->code = code;
		cmd.tempo->ticksPerMinute = (float)va_arg(args, double);
		break;
	case Player_Cmd_assign:
		cmd.assign = MALLOC(PLAYER_CMD_ASSIGN, 1);
		cmd.assign->code = code;
		cmd.assign->target = (UINT8)va_arg(args, int);
		cmd.assign->sequence = (UINT8)va_arg(args, int);
		cmd.assign->status = (UINT8)va_arg(args, int);
		break;
	//case Player_Cmd_create:
	//	cmd.create = MALLOC(PLAYER_CMD_CREATE, 1);
	//	cmd.create->code = code;
	//	cmd.create->adapterId = (UINT8)va_arg(args, int);
	//	cmd.create->userDataBlockId = (UINT8)va_arg(args, int);
	//	break;
	}
	va_end(args);
	return cmd.base;
}
int PlayerAdapter::getCommandParameters(PLAYER_COMMAND command, double* parameters) {
	int count = -1;
	PLAYER_COMMAND_ALL cmd;
	cmd.base = command;
	switch (cmd.base[0]) {
	case Player_Cmd_setTempo:		// db code, float tpm
		parameters[0] = (double)cmd.tempo->ticksPerMinute;
		count = 1;
		break;
	case Player_Cmd_assign:	// db code, db targetId, db sequenceId, db status
		parameters[0] = (double)cmd.assign->target;
		parameters[1] = (double)cmd.assign->sequence;
		parameters[2] = (double)cmd.assign->status;
		count = 3;
		break;
	//case Player_Cmd_create:
	//	parameters[0] = (int)cmd.create->adapterId;
	//	parameters[1] = (int)cmd.create->userDataBlockId;
	//	break;
	default:
		//AbstractAdapter::dumpCommand(command, buffer);
		break;
	}
	return count;
}
int PlayerAdapter::matchCommand(int filter, PLAYER_COMMAND cmd) {
	return (filter & 0xFF) == cmd[0];
}
char* PlayerAdapter::logCommand(PLAYER_COMMAND command) {
	char* str = NULL;
	PLAYER_COMMAND_ALL cmd;
	cmd.base = command;
	switch (cmd.base[0]) {
	case Player_Cmd_setTempo:	// tempo(20.0 )
		str = MALLOC(char, 32);
		sprintf_s(str, 32, "tempo(%f)", cmd.tempo->ticksPerMinute);
		break;
	case Player_Cmd_assign:		// assign(  1,  2, 0x80)
		str = MALLOC(char, 24);
		sprintf_s(str, 24, "assign(% 3d, % 3d, 0x%02X)", cmd.assign->target, cmd.assign->sequence, cmd.assign->status);
		break;
	//case Player_Cmd_create:		// create(  1,  2)
	//	str = MALLOC(char, 24);
	//	sprintf_s(str, 24, "create(% 3d, % 3d)", cmd.create->adapterId, cmd.create->userDataBlockId);
	//	break;
	default:
		str = IAdapter::logCommand(command);
		break;
	}
	return str;
}
int PlayerAdapter::dumpCommand(PLAYER_COMMAND command, Buffer* buffer) {
	PLAYER_COMMAND_ALL cmd;
	cmd.base = command;
	UINT8 tmp[64];
	int length = 1;
	tmp[0] = cmd.base[0];
	switch (cmd.base[0]) {
	case Player_Cmd_setTempo:	// db code, float fpm, float tpf
		*(float*)&tmp[length] = cmd.tempo->ticksPerMinute; length += sizeof(float);
		break;
	case Player_Cmd_assign:		// db code, db targetId, db sequenceId, db status
		tmp[length++] = cmd.assign->target;
		tmp[length++] = cmd.assign->sequence;
		tmp[length++] = cmd.assign->status;
		break;
	//case Player_Cmd_create:
	//	tmp[length++] = cmd.create->adapterId;
	//	tmp[length++] = cmd.create->userDataBlockId;
	//	break;
	default:
		//AbstractAdapter::dumpCommand(command, buffer);
		break;
	}
	if (buffer != NULL) {
		buffer->append(tmp, length);
	}
	return length;
}

// Editor
Target* PlayerAdapter::createTarget(int id, UINT8* data) {
	return NULL;
}
#endif

NS_PLAYER_END