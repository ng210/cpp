#ifndef __PLAYER_IADAPTER_H
#define __PLAYER_IADAPTER_H

#include "playerdef.h"
#include "abstractchannel.h"
#include "utils/buffer.h"

NS_FW_BASE_USE
NS_PLAYER_BEGIN

typedef struct ABSTRACT_ADAPTER_COMMAND_ {
	UINT8 code;
} ABSTRACT_ADAPTER_COMMAND;

class Player;
// Interface
class IAdapter {
public:
	virtual int getId() = 0;
	virtual int processCommand(void* object, PLAYER_COMMAND cmd) = 0;
	virtual void setTempo(void *object, float ticksPerSecond) = 0;
	//virtual Target* createTarget(void* initData) = 0;

	// Misc. methods
	virtual int initialize(void* data, Player* player) = 0;
	virtual PLAYER_COMMAND createCommand(int code, ...) = 0;
	virtual int getCommandParameters(PLAYER_COMMAND cmd, double* parameters) = 0;
	virtual int matchCommand(int filter, PLAYER_COMMAND cmd) = 0;
	virtual char* logCommand(PLAYER_COMMAND cmd);
	virtual int dumpCommand(PLAYER_COMMAND cmd, Buffer* buffer) = 0;
	static PLAYER_COMMAND createEndCommand();

};

NS_PLAYER_END

#endif