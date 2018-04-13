#ifndef __PLAYER_ABSTRACT_ADAPTER_H
#define __PLAYER_ABSTRACT_ADAPTER_H

#include "playerdef.h"
#include "playercommand.h"
//#include "collection/array.h"

//NS_FW_BASE_USE
NS_PLAYER_BEGIN

// Interface 
class AbstractAdapter {
public:
	virtual int prepareObject(void* object) = 0;
	virtual int processCommand(void* object, PLAYER_COMMAND* cmd) = 0;
};

class Target {
public:
	void* object;
	AbstractAdapter* adapter;
	Target(void* object, AbstractAdapter* adapter) : object(object), adapter(adapter) {};
};

NS_PLAYER_END

#endif