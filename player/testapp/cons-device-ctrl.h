#ifndef __CONS_DEVICE_CTRL_H
#define __CONS_DEVICE_CTRL_H

#include "player/src/device-ctrl.h"
#include "player/test/cons-device-ext.h"

using namespace PLAYER;

class ConsDeviceCtrl : public DeviceCtrl {
	static char* windowClassName_;
	static ATOM windowClass_;
public:
	ConsDeviceCtrl(ConsDeviceExt*);
	~ConsDeviceCtrl();

	char* const registerWindowClass();

	void create(Window* parent, char* name);

	void loadBackgroundImage();

	//LRESULT onPaint();
};

#endif