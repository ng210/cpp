#ifndef __CONS_ADAPTER_H
#define __CONS_ADAPTER_H

#include "utils/iconsole.h"
#include "player/src/adapter.h"

using namespace PLAYER;

typedef enum ConsDevices {
	DeviceCons = 1
} ConsDevices;

typedef IConsole* const (GetConsole)();

class ConsAdapter : public Adapter {
	static AdapterInfo adapterInfo_;
	PROP_R(dword, initData);
public:
	ConsAdapter();
	virtual ~ConsAdapter();

	AdapterInfo* const getInfo();
	void initialize(byte** pData);
	Device* createDevice(int deviceType);
	void writeToStream(Stream* data);
	
	static GetConsole* getConsole;
};

#endif
