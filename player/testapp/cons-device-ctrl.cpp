#include "player/testapp/cons-device-ctrl.h"

char* ConsDeviceCtrl::windowClassName_ = "ConsDeviceCtrl";
ATOM ConsDeviceCtrl::windowClass_;

ConsDeviceCtrl::ConsDeviceCtrl(ConsDeviceExt* consDeviceExt) : DeviceCtrl(consDeviceExt) {

}

ConsDeviceCtrl::~ConsDeviceCtrl() {

}

char* const ConsDeviceCtrl::registerWindowClass() {
	if (ConsDeviceCtrl::windowClass_ == 0) {
		ConsDeviceCtrl::windowClass_ = Window::registerClass(ConsDeviceCtrl::windowClassName_, NULL, 0);
	}
	return ConsDeviceCtrl::windowClassName_;
}

void ConsDeviceCtrl::create(Window* parent, char* name, LONG style, DWORD exStyle) {
	DeviceCtrl::create(parent, name, style, exStyle);

	//setSize(3*64+8, 48+8);
}

void ConsDeviceCtrl::loadBackgroundImage() {
	SYSFN(hBackgroundImage_, LoadImage(NULL, "cons-ctrl.bmp", IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT | LR_LOADFROMFILE));
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