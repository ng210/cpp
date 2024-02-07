#ifndef __CONS_DEVICE_CTRL_H
#define __CONS_DEVICE_CTRL_H

#include "player/src/device-ctrl.h"
#include "player/test/cons-device-ext.h"

using namespace PLAYER;

class ConsDeviceCtrl : public DeviceCtrl {
	static char* ConsDeviceCtrl::windowClassName_;
	static ATOM ConsDeviceCtrl::windowClass_;
public:
	ConsDeviceCtrl(ConsDeviceExt*);
	~ConsDeviceCtrl();

	char* const registerWindowClass();

	void create(Window* parent, char* name, LONG style = 0, DWORD exStyle = 0);

	void loadBackgroundImage();

	//LRESULT onPaint();
};

#endif