#ifndef __CONS_ADAPTER_H
#define __CONS_ADAPTER_H

#include "adapter.h"

using namespace PLAYER;

typedef enum ConsDevices {
	DeviceCons = 1
} ConsDevices;

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
};

#endif
