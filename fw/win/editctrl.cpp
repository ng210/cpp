#include "win/editctrl.h"

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

WndClass EditCtrl::WndClass_("EDIT");

EditCtrl::EditCtrl() {
}

void EditCtrl::create(Window* parent, char* name, LONG style, DWORD exStyle) {
	if (style == 0) {
		style = WS_BORDER | ES_LEFT;
	}
	//style |= WS_CHILD | WS_VISIBLE;

	Ctrl::create(EditCtrl::WndClass_, parent, name, style, exStyle);
}

EditCtrl::~EditCtrl() {
}

NS_FW_WIN_END