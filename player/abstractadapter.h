#ifndef __PLAYER_ABSTRACT_ADAPTER_H
#define __PLAYER_ABSTRACT_ADAPTER_H

#include "playerdef.h"
#include "abstractchannel.h"
#include "utils/buffer.h"

NS_FW_BASE_USE
NS_PLAYER_BEGIN

typedef struct ABSTRACT_ADAPTER_COMMAND_ {
	BYTE code;
} ABSTRACT_ADAPTER_COMMAND;

// Interface 
class AbstractAdapter {
protected: PROP_R(int, id);
public:
	virtual int prepareObject(void* object) = 0;
	virtual int processCommand(void* object, PLAYER_COMMAND cmd) = 0;
	virtual PLAYER_COMMAND createCommand(int code, ...) = 0;
#ifdef _DEBUG
	virtual char* logCommand(PLAYER_COMMAND cmd);
#endif
	virtual int dumpCommand(PLAYER_COMMAND cmd, Buffer* buffer) = 0;
	PLAYER_COMMAND createEndCommand();

	//PLAYER_CHANNEL_CREATE* createChannel;
};

NS_PLAYER_END

#endif