#ifndef __PLAYER_ABSTRACT_ADAPTER_H
#define __PLAYER_ABSTRACT_ADAPTER_H

#include "playerdef.h"
//#include "collection/array.h"

//NS_FW_BASE_USE
NS_PLAYER_BEGIN

typedef unsigned char COMMAND_ARGUMENT;

#pragma pack(push, 1)
typedef struct PLAYER_COMMAND_S {
	unsigned short delta;
	unsigned char code;
	unsigned char argc;
	unsigned char args;
} PLAYER_COMMAND;
#pragma pack(pop)

typedef union PLAYER_COMMAND_U_ {
	PLAYER_COMMAND_S* s;
	unsigned char* p;
} PLAYER_COMMAND_U;


class PlayerCommand {
public:
	size_t delta_;
	size_t code_;
	COMMAND_ARGUMENT* args_;

	//PlayerCommand(size_t delta, size_t code, COMMAND_ARGUMENT* args);
	PlayerCommand(size_t delta, size_t code, COMMAND_ARGUMENT* args);
	~PlayerCommand();
};

// Interface 
class AbstractAdapter {
public:
	virtual int prepareObject(void* object) = 0;
	virtual int processCommand(void* target, PLAYER_COMMAND* cmd) = 0;
};

class Target {
public:
	void* object;
	AbstractAdapter* adapter;
	Target(void* object, AbstractAdapter* adapter) : object(object), adapter(adapter) {}
};

NS_PLAYER_END

#endif