#ifndef __PLAYER_TARGET_H
#define __PLAYER_TARGET_H

#include "playerdef.h"

NS_PLAYER_BEGIN

class AbstractAdapter;
class Target {
public:
	void* object;
	AbstractAdapter* adapter;
public:
	Target(void* o, AbstractAdapter* a) : object(o), adapter(a) {}
}; 

NS_PLAYER_END

#endif