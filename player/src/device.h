#ifndef __PLAYER_DEVICE_H
#define __PLAYER_DEVICE_H

#include "basedef.h"

namespace PLAYER {

	class Device {
	private: PROP_R(int, refreshRate);
	private: PROP(int, type);
	public:
		Device() : refreshRate_(25), type_(0) {}
		virtual ~Device() {};
		virtual void setRefreshRate(float fps) = 0;
	};
}

#endif