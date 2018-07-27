#ifndef __PLAYER_PLAYER_ADAPTER_H
#define __PLAYER_PLAYER_ADAPTER_H

#include "iadapter.h"

NS_PLAYER_BEGIN

#define PLAYER_ADAPTER_ID 0x00

/******************************************************************************
* Commands
*****************************************************************************/
enum PlayerCommands : UINT8 {
	Player_Cmd_setTempo,		// sets the frame rate (fps)
	Player_Cmd_assign,			// assigns a sequence to a channel
	Player_Cmd_create,			// create target

	Player_Cmd_Count
};

typedef struct PLAYER_CMD_TEMPO_ : ABSTRACT_ADAPTER_COMMAND_ {
	float ticksPerMinute;
} PLAYER_CMD_TEMPO;

typedef struct PLAYER_CMD_ASSIGN_ : ABSTRACT_ADAPTER_COMMAND_ {
	UINT8 target;
	UINT8 sequence;
	UINT8 status;
} PLAYER_CMD_ASSIGN;

//typedef struct PLAYER_CMD_CREATE_ : ABSTRACT_ADAPTER_COMMAND_ {
//	UINT8 adapterId;
//	UINT8 userDataBlockId;
//} PLAYER_CMD_CREATE;

typedef union PLAYER_COMMAND_ALL_ {
	PLAYER_CMD_TEMPO* tempo;
	PLAYER_CMD_ASSIGN* assign;
	//PLAYER_CMD_CREATE* create;
	PLAYER_COMMAND base;
} PLAYER_COMMAND_ALL;

class PlayerAdapter : public IAdapter {
public:
	int initialize(void* data, Player* player);
	int getId();
	int processCommand(void* object, PLAYER_COMMAND cmd);
	void setTempo(void *object, float ticksPerSecond);
	//Target* createTarget(void* initData);

	// Misc. methods

	PLAYER_COMMAND createCommand(int code, ...);
	int getCommandParameters(PLAYER_COMMAND cmd, double* parameters);
	int matchCommand(int filter, PLAYER_COMMAND cmd);
	char* logCommand(PLAYER_COMMAND command);
	int dumpCommand(PLAYER_COMMAND command, Buffer* buffer);
};

NS_PLAYER_END

#endif