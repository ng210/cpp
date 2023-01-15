#include "device-type.h"
#include "device.h"
#include "base/str.h"
#include "player.h"
#include "player-device.h"


namespace PLAYER {

	DeviceType::DeviceType(int type, char* name) : type_(type) {
		parent_ = NULL;
		strncpy(name_, 15, name);
		isInitialized_ = false;
		instances_.init(16);
	}

	DeviceType::~DeviceType() {}

	void DeviceType::initialize(byte** pData) {
		if (pData != NULL && *pData != NULL) {
			prepareContext(pData);
			// create devices
			var playerDevice = (PlayerDevice*)parent_->device();
			byte*& data = *pData;
			var deviceCount = *data++;
			for (var i = 0; i < deviceCount; i++) {
				playerDevice->addDevice(type_, *data++, pData);
			}
			isInitialized_ = true;
		}
	}

	Device* DeviceType::createDevice(int type, byte** pData) {
		var dev = createDeviceImpl(type);
		var playerDevice = (Device*)parent_->device();
		dev->parent(playerDevice);
		dev->type(type);
		dev->deviceType(this);
		instances_.add(dev);
		dev->initialize(pData);
		return dev;
	}

	void DeviceType::prepareContext(byte** pData) {

	}

	int DeviceType::writeInitDataToStream(Stream* stream) {
		return 0;
	}

	int DeviceType::writeToStream(Stream* stream) {
		int len = writeInitDataToStream(stream);
		stream->writeByte(instances_.length()); len++;
		for (var i = 0; i < instances_.length(); i++) {
			var dev = (Device*)instances_.getAt(i);
			stream->writeByte(dev->type()); len++;
			len += dev->writeInitDataToStream(stream);
		}

		return len;
	}

	char* const DeviceType::name() {
		return name_;
	}
}