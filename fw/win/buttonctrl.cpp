#include "win/buttonctrl.h"

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

WndClass ButtonCtrl::WndClass_("BUTTON");

WndClass ButtonCtrl::getWindowClass() {
	return ButtonCtrl::WndClass_;
}

//void ButtonCtrl::createWindow(Window* parent, char* name, LONG style, DWORD exStyle) {
//	Ctrl::createWindow(ButtonCtrl::WndClass_, parent, name, style, exStyle);
//}

NS_FW_WIN_END