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

typedef struct PLAYER_CMD_TEMPO_ : ADAPTER_COMMAND_BASE {
	float ticksPerMinute;
} PLAYER_CMD_TEMPO;

typedef struct PLAYER_CMD_ASSIGN_ : ADAPTER_COMMAND_BASE {
	UINT8 target;
	UINT8 adapter;
	UINT8 sequence;
	UINT8 status;
} PLAYER_CMD_ASSIGN;

typedef union PLAYER_COMMAND_ALL_ {
	PLAYER_CMD_TEMPO* tempo;
	PLAYER_CMD_ASSIGN* assign;
	PLAYER_COMMAND_U base;
} PLAYER_COMMAND_ALL;

class PlayerAdapter : public IAdapter {
	static const ADAPTER_INFO adapterInfo_;
	//static void initialize();
	//static void destroy();
	//static IAdapter* create(UINT8** data);
	static PlayerAdapter instance_;
public:
	void createObjects(PArray* targets, void* data);
	const ADAPTER_INFO& getInfo();
	static const IAdapter& getInstance();
	void prepareContext(void* data);
	int processCommand(void* target, PLAYER_COMMAND_U data);
	void updateRefreshRate(void* target, float data);

	//size_t fill(void* buffer, size_t start, size_t end);

#ifdef _EDITOR
	// editor extensions
	PLAYER_COMMAND createCommand(int code, ...);
	int getCommandParameters(PLAYER_COMMAND cmd, double* parameters);
	int matchCommand(int filter, PLAYER_COMMAND cmd);
	char* logCommand(PLAYER_COMMAND command);
	int dumpCommand(PLAYER_COMMAND command, Buffer* buffer);
	Target* createTarget(int id, UINT8* data);
#endif
};

NS_PLAYER_END

#endif