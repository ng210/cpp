#include <stdio.h>
#include <stdarg.h>
#include "playeradapter.h"
#include "player.h"

NS_PLAYER_BEGIN

PlayerAdapter::PlayerAdapter() {
	id_ = PLAYER_ADAPTER_ID;
}

int PlayerAdapter::prepareObject(void* object) {
	return 0;
}
int PlayerAdapter::processCommand(void* object, PLAYER_COMMAND command) {
	Player* player = (Player*)object;
	switch (command[0]) {
	case Player_Cmd_setTempo:	// fps, tps
		float refreshRate;
		refreshRate = ((PLAYER_CMD_SET_TEMPO*)command)->framePerMinute * ((PLAYER_CMD_SET_TEMPO*)command)->ticksPerFrame / 60.0f;
		ARRAY_FOREACH(player->channels(), Target* target = ((AbstractChannel*)value)->target(); target->adapter->setTempo(target->object, refreshRate));
		break;
	case Player_Cmd_assign:		// target, sequence, status
		Target* target = (Target*)player->targets()->getAt(((PLAYER_CMD_ASSIGN*)command)->target);
		PLAYER_SEQUENCE sequence;
		sequence = (UINT8*)player->sequences()->getAt(((PLAYER_CMD_ASSIGN*)command)->sequence);
		size_t status = ((PLAYER_CMD_ASSIGN*)command)->status;
		AbstractChannel* chn = NULL;
		// get an inactive channel
		size_t ix = -1;
		ARRAY_FOREACH(player->channels(), if (!((AbstractChannel*)value)->isActive()) { ix = i; chn = (AbstractChannel*)value; break; });
		if (ix == -1) {
			// create new channel
			AbstractChannel* chn = Player::createChannel();
			chn->init(/*player,*/ player->channels()->length(), target, sequence, status);
			player->addChannel(chn);
			//chn->id_ = NEW_(String, ix);
		} else {
			chn->assign(/*player,*/ target, sequence, status);
		}
		break;
	}
	return 0;
}

void PlayerAdapter::setTempo(void *object, float ticksPerSecond) {
	((Player*)object)->refreshRate(ticksPerSecond);
}

PLAYER_COMMAND PlayerAdapter::createCommand(int code, ...) {
	PLAYER_COMMAND cmd = NULL;
	va_list args;
	va_start(args, code);
	switch (code) {
	case Player_Cmd_setTempo:
		cmd = (UINT8*)MALLOC(PLAYER_CMD_SET_TEMPO, 1);
		((PLAYER_CMD_SET_TEMPO*)cmd)->code = code;
		((PLAYER_CMD_SET_TEMPO*)cmd)->framePerMinute = (float)va_arg(args, double);	// framePerMinute
		((PLAYER_CMD_SET_TEMPO*)cmd)->ticksPerFrame = (float)va_arg(args, double);	// ticksPerFrame
		break;
	case Player_Cmd_assign:
		cmd = (UINT8*)MALLOC(PLAYER_CMD_ASSIGN, 1);
		((PLAYER_CMD_ASSIGN*)cmd)->code = code;
		((PLAYER_CMD_ASSIGN*)cmd)->target = (UINT8)va_arg(args, int);	// id
		((PLAYER_CMD_ASSIGN*)cmd)->sequence = (UINT8)va_arg(args, int);	// value
		((PLAYER_CMD_ASSIGN*)cmd)->status = (UINT8)va_arg(args, int);	// value
		break;
	}
	va_end(args);
	return cmd;
}
#ifdef _DEBUG
char* PlayerAdapter::logCommand(PLAYER_COMMAND command) {
	char* str = NULL;
	PLAYER_CMD_SET_TEMPO* cmdTempo;
	PLAYER_CMD_ASSIGN* cmdAssign;
	switch (command[0]) {
	case Player_Cmd_setTempo:		// tempo( 20,  8)
		cmdTempo = (PLAYER_CMD_SET_TEMPO*)command;
		str = MALLOC(char, 64);
		sprintf_s(str, 64, "tempo(%f, %f)", cmdTempo->framePerMinute, cmdTempo->ticksPerFrame);
		break;
	case Player_Cmd_assign:	// assign(  1, 2, 0x80)
		cmdAssign = (PLAYER_CMD_ASSIGN*)command;
		str = MALLOC(char, 24);
		sprintf_s(str, 24, "assign(%.3d, %.3d, 0x%02X)", cmdAssign->target, cmdAssign->sequence, cmdAssign->status);
		break;
	default:
		str = AbstractAdapter::logCommand(command);
		break;
	}
	return str;
}
#endif

int PlayerAdapter::dumpCommand(PLAYER_COMMAND command, Buffer* buffer) {
	PLAYER_CMD_SET_TEMPO* cmdTempo;
	PLAYER_CMD_ASSIGN* cmdAssign;
	UINT8 tmp[64];
	int length = 1;
	tmp[0] = command[0];
	switch (command[0]) {
	case Player_Cmd_setTempo:		// db code, float fpm, float tpf
		cmdTempo = (PLAYER_CMD_SET_TEMPO*)command;
		*(float*)&tmp[length] = cmdTempo->framePerMinute; length += sizeof(float);
		*(float*)&tmp[length] = cmdTempo->ticksPerFrame; length += sizeof(float);
		break;
	case Player_Cmd_assign:	// db code, db targetId, db sequenceId, db status
		cmdAssign = (PLAYER_CMD_ASSIGN*)command;
		tmp[length++] = cmdAssign->target;
		tmp[length++] = cmdAssign->sequence;
		tmp[length++] = cmdAssign->status;
		break;
	default:
		//AbstractAdapter::dumpCommand(command, buffer);
		break;
	}
	if (buffer != NULL) {
		buffer->append(tmp, length);
	}
	return length;
}
NS_PLAYER_END