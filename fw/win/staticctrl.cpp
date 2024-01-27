#include "win/staticctrl.h"

NS_FW_BASE_USE
NS_FW_WIN_USE

char* StaticCtrl::windowClassName_ = "STATIC";

char* const StaticCtrl::registerWindowClass() {
	return StaticCtrl::windowClassName_;
}
