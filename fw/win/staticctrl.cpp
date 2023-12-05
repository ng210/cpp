#include "win/staticctrl.h"

NS_FW_BASE_USE
NS_FW_WIN_USE

WndClass StaticCtrl::WndClass_("STATIC");

WndClass StaticCtrl::getWindowClass() {
	return StaticCtrl::WndClass_;
}
