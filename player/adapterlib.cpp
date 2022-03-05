#include "adapterlib.h"
#include "base/memory.h"

NS_PLAYER_BEGIN

Map AdapterLib::adapters(sizeof(ADAPTER_INFO::uid), MAP_USE_REF, Map::hashingItem);

void AdapterLib::addAdapter(IAdapter* adapter) {
	UINT32 uid = adapter->getInfo().uid;
	if (!AdapterLib::adapters.containsKey(&uid)) {
		AdapterLib::adapters.put(&uid, adapter);
	}
}

IAdapter* const AdapterLib::getAdapter(int uid) {
	return (IAdapter*)AdapterLib::adapters.get(&uid);
}

Map& AdapterLib::getAdapters() {
	return AdapterLib::adapters;
}

NS_PLAYER_END