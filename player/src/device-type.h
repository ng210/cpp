#ifndef __PLAYER_DEVICE_TYPE_H
#define __PLAYER_DEVICE_TYPE_H

#include "basedef.h"
#include "stream.h"
#include "collection/parray.h"
#include "player-defs.h"

NS_FW_BASE_USE

namespace PLAYER {

	class Device;
	struct DeviceData;
	typedef Device* (DeviceCreator)();

	class Player;
	class PlayerDevice;

	class DeviceType {
	protected: PROP(Player*, parent);
	protected: PROP_R(int, type);
	protected: PROP_R(PArray, instances);
	protected: char name_[16];
	protected: PROP_R(bool, isInitialized);
	//protected: PROP_R(DeviceCreator*, creator);
	public:
		DeviceType(const int type, char* const name);
		virtual ~DeviceType();

		virtual void initialize(byte** pData = NULL);
		virtual void prepareContext(byte** pData = NULL);
		Device* createDevice(int type, byte** pData = NULL);
		virtual Device* createDeviceImpl(int type) = 0;

		char* const name();
		virtual int writeToStream(Stream* stream);
		virtual int writeInitDataToStream(Stream* stream);
	};
}

#endif
