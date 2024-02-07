#include "win/buttonctrl.h"

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

char* ButtonCtrl::windowClassName_ = "BUTTON";

char* const ButtonCtrl::registerWindowClass() {
	return ButtonCtrl::windowClassName_;
}

NS_FW_WIN_END