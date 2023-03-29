#include "win/staticctrl.h"

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

WndClass StaticCtrl::WndClass_("STATIC");

void StaticCtrl::create(Window* parent, char* name, LONG style, DWORD exStyle) {
	Ctrl::create(StaticCtrl::WndClass_, parent, name, style, exStyle);
}

NS_FW_WIN_END