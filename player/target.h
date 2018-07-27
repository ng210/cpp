#ifndef __PLAYER_TARGET_H
#define __PLAYER_TARGET_H

#include "playerdef.h"

NS_PLAYER_BEGIN

class IAdapter;
class Target {
public:
	void* object;
	IAdapter* adapter;
public:
	Target(void* o, IAdapter* a) : object(o), adapter(a) {}
}; 

NS_PLAYER_END

#endif