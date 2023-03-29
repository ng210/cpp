#include "win/buttonctrl.h"

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

WndClass ButtonCtrl::WndClass_("BUTTON");

void ButtonCtrl::create(Window* parent, char* name, LONG style, DWORD exStyle) {
	Ctrl::create(ButtonCtrl::WndClass_, parent, name, style, exStyle);
}

NS_FW_WIN_END