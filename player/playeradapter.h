#ifndef __PLAYER_PLAYER_ADAPTER_H
#define __PLAYER_PLAYER_ADAPTER_H

#include "abstractadapter.h"

NS_PLAYER_BEGIN

#define PLAYER_ADAPTER_ID 0x00

/******************************************************************************
* Commands
*****************************************************************************/
enum PlayerCommands : BYTE {
	Player_Cmd_setTempo,		// sets the frame rate (fps)
	Player_Cmd_assign,			// assigns a sequence to a channel

	Player_Cmd_Count
};

typedef struct PLAYER_CMD_SET_TEMPO_ : ABSTRACT_ADAPTER_COMMAND_ {
	float framePerMinute;
	float ticksPerFrame;
} PLAYER_CMD_SET_TEMPO;

typedef struct PLAYER_CMD_ASSIGN_ : ABSTRACT_ADAPTER_COMMAND_ {
	BYTE target;
	BYTE sequence;
	BYTE status;
} PLAYER_CMD_ASSIGN;

class PlayerAdapter : public AbstractAdapter {
public:
	PlayerAdapter();
	int prepareObject(void* object);
	int processCommand(void* object, PLAYER_COMMAND cmd);
	PLAYER_COMMAND createCommand(int code, ...);
#ifdef _DEBUG
	char* logCommand(PLAYER_COMMAND command);
#endif
	int dumpCommand(PLAYER_COMMAND command, Buffer* buffer);
	//int getArgCount(PLAYER_COMMAND cmd);
};

NS_PLAYER_END

#endif