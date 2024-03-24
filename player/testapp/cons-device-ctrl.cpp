#include "player/testapp/cons-device-ctrl.h"

char* ConsDeviceCtrl::windowClassName_ = "ConsDeviceCtrl";
ATOM ConsDeviceCtrl::windowClass_ = 0;

ConsDeviceCtrl::ConsDeviceCtrl(ConsDeviceExt* consDeviceExt) : DeviceCtrl(consDeviceExt) {
	// background, foreground, frame, text
	colors[0] = 0x00e0a080; colors[1] = 0x00e08040;
	colors[2] = 0x00802008; colors[3] = 0x00400000;
	borderWidth_ = 1;
}

ConsDeviceCtrl::~ConsDeviceCtrl() {

}

char* const ConsDeviceCtrl::registerWindowClass() {
	if (ConsDeviceCtrl::windowClass_ == 0) {
		ConsDeviceCtrl::windowClass_ = Window::registerClass(ConsDeviceCtrl::windowClassName_, NULL, 0);
	}
	return ConsDeviceCtrl::windowClassName_;
}

void ConsDeviceCtrl::create(Window* parent, char* name) {
	DeviceCtrl::create(parent, name);

	//setSize(3*64+8, 48+8);
}

void ConsDeviceCtrl::loadBackgroundImage() {
	SYSFN(hBackgroundImage_, LoadImage(NULL, "test-ctrl.bmp", IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT | LR_LOADFROMFILE));
}


//LRESULT ConsDeviceCtrl::onPaint() {
//	PAINTSTRUCT ps;
//	var hdc = BeginPaint(hWnd_, &ps);
//
//	FillRect(hdc, &rect_, hBgBrush_ );
//
//
//	EndPaint(hWnd_, &ps);
//	return 0;
//}