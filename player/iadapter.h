#ifndef __PLAYER_IADAPTER_H
#define __PLAYER_IADAPTER_H

#include "playerdef.h"
#include "abstractchannel.h"
#include "utils/buffer.h"
#include "collection/map.h"

NS_FW_BASE_USE
NS_PLAYER_BEGIN

typedef struct ABSTRACT_ADAPTER_COMMAND_ {
	UINT8 code;
} ABSTRACT_ADAPTER_COMMAND;

class IAdapter;
typedef void (ADAPTERCALLBACK)();
typedef IAdapter* (CREATEADAPTER)(UINT8**);
typedef Array* (GETADAPTERINFOS)();

typedef struct ADAPTER_INFO_ {
	UINT32 id;
	char name[32];
	ADAPTERCALLBACK* initialize;
	ADAPTERCALLBACK* destroy;
	CREATEADAPTER* create;
	Map* targetTypes;
} ADAPTER_INFO;

typedef struct ADAPTER_DATA_ {
	ADAPTER_INFO* info;
	int udbId;
} ADAPTER_DATA;

typedef struct ADAPTER_TARGET_TYPE_ {
	int id;
	char name[32];
} ADAPTER_TARGET_TYPE;

class Player;
// Interface
class IAdapter {
public:
	virtual const ADAPTER_INFO* getInfo() = 0;
	//virtual UINT32 createTargets(Player* player) = 0;

	virtual int processCommand(void* object, PLAYER_COMMAND cmd) = 0;
	virtual void setTempo(void *object, float ticksPerSecond) = 0;
	virtual size_t fill(void* buffer, size_t start, size_t end) = 0;

	// Misc. methods
	virtual PLAYER_COMMAND createCommand(int code, ...) = 0;
	virtual int getCommandParameters(PLAYER_COMMAND cmd, double* parameters) = 0;
	virtual int matchCommand(int filter, PLAYER_COMMAND cmd) = 0;
	virtual char* logCommand(PLAYER_COMMAND cmd);
	virtual int dumpCommand(PLAYER_COMMAND cmd, Buffer* buffer) = 0;
	static PLAYER_COMMAND createEndCommand();

	// Editor
	virtual Target* createTarget(int id, UINT8* data) = 0;
};

NS_PLAYER_END

#endif