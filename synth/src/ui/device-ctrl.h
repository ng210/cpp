#ifndef __DEVICE_CTRL_H
#define __DEVICE_CTRL_H

#include "win/ctrl.h"
#include "player/src/device.h"

NS_FW_WIN_USE

namespace PLAYER {
	class DeviceCtrl : public Ctrl {

	protected: PROP_R(Device*, device);
	public:
		DeviceCtrl(Device* device);
		virtual ~DeviceCtrl();

		void create(Window* parent, char* name);
	};
}
#endif