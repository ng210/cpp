#include "win/buttonctrl.h"

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

char* ButtonCtrl::windowClassName_ = "BUTTON";

char* const ButtonCtrl::registerWindowClass() {
	return ButtonCtrl::windowClassName_;
}

//void ButtonCtrl::createWindow(Window* parent, char* name, LONG style, DWORD exStyle) {
//	Ctrl::createWindow(ButtonCtrl::WndClass_, parent, name, style, exStyle);
//}

NS_FW_WIN_END