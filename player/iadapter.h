#ifndef __PLAYER_IADAPTER_H
#define __PLAYER_IADAPTER_H

#include "playerdef.h"
#include "collection/parray.h"

NS_FW_BASE_USE
NS_PLAYER_BEGIN

typedef struct ADAPTER_INFO_ {
	UINT32 uid;
	char name[32];
	//ADAPTERCALLBACK* initialize;
	//ADAPTERCALLBACK* destroy;
	//CREATEADAPTER* create;
	//Map* targetTypes;
} ADAPTER_INFO;


class IAdapter {
public:
	virtual const ADAPTER_INFO& getInfo() = 0;
	virtual void createObjects(PArray* targets, void* data) = 0;
	virtual void prepareContext(void* data) = 0;
	virtual int processCommand(void* target, PLAYER_COMMAND_U data) = 0;
	virtual void updateRefreshRate(void* target, float ticksPerSecond) = 0;

#ifdef _EDITOR
	// editor extensions
	virtual PLAYER_COMMAND_U createCommand(int id, ...) = 0;
	int getCommandParameters(PLAYER_COMMAND cmd, double* parameters);
	int matchCommand(int filter, PLAYER_COMMAND cmd);
	char* logCommand(PLAYER_COMMAND command);
	int dumpCommand(PLAYER_COMMAND command, Buffer* buffer);
	Target* createTarget(int id, UINT8* data);
#endif

};

//class IAdapter;
//typedef void (ADAPTERCALLBACK)();
//typedef IAdapter* (CREATEADAPTER)(UINT8**);
//typedef Array* (GETADAPTERINFOS)();
//
//typedef struct ADAPTER_INFO_ {
//	UINT32 uid;
//	char name[32];
//	ADAPTERCALLBACK* initialize;
//	ADAPTERCALLBACK* destroy;
//	CREATEADAPTER* create;
//	Map* targetTypes;
//} ADAPTER_INFO;
//
//typedef struct ADAPTER_DATA_ {
//	ADAPTER_INFO* info;
//	int udbId;
//} ADAPTER_DATA;
//
//typedef struct ADAPTER_TARGET_TYPE_ {
//	int id;
//	char name[32];
//} ADAPTER_TARGET_TYPE;
//
//class Player;
//class Target;
//// Interface
//class IAdapter {
//public:
//	virtual const ADAPTER_INFO* getInfo() = 0;
//	virtual void initialize(UINT8* data) = 0;
//	virtual void createObjects(PArray targets, UINT8* data) = 0;
//	virtual void prepareContext(UINT8* data) = 0;
//	virtual int processCommand(Target* target, UINT8* data) = 0;
//	virtual void updateRefreshRate(Target* target, UINT8* data) = 0;
//
//	// Editor
//	//virtual Target* createTarget(int id, UINT8* data) = 0;
//	//virtual PLAYER_COMMAND createCommand(int code, ...) = 0;
//	//virtual int getCommandParameters(PLAYER_COMMAND cmd, double* parameters) = 0;
//	//virtual int matchCommand(int filter, PLAYER_COMMAND cmd) = 0;
//	//virtual char* logCommand(PLAYER_COMMAND cmd);
//	//virtual int dumpCommand(PLAYER_COMMAND cmd, Buffer* buffer) = 0;
//	//static PLAYER_COMMAND createEndCommand();
//
//};

NS_PLAYER_END

#endif