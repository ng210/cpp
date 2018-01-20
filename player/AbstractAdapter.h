#ifndef __PLAYER_ABSTRACT_ADAPTER_H
#define __PLAYER_ABSTRACT_ADAPTER_H

#include "Player.h"
#include "collection/Array.h"

NS_FW_BASE_USE
NS_PLAYER_BEGIN

class PlayerCommand : public Object {
public:
	float delta;
	int cmd;
	Array* args;
};

// Interface 
class AbstractAdapter {
public:
	virtual int prepareObject(void* object);
	virtual int processCommand(void* target, PlayerCommand* cmd);
};

class Target : public Object {
public:
	void* object;
	AbstractAdapter* adapter;
	Target(void* object, AbstractAdapter* adapter) : object(object), adapter(adapter) {}
};

NS_PLAYER_END

#endif