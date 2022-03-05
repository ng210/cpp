#ifndef __PLAYER_ADAPTERLIB_H
#define __PLAYER_ADAPTERLIB_H

#include "iadapter.h"
#include "collection/map.h"

NS_FW_BASE_USE
NS_PLAYER_BEGIN

class AdapterLib {
	static Map adapters;
public:
	static void addAdapter(IAdapter* adapter);
	static IAdapter* const getAdapter(int uid);
	static Map& getAdapters();
};

NS_PLAYER_END

#endif