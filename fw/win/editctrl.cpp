#include "win/editctrl.h"

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

char* EditCtrl::windowClassName_ = "EDIT";

EditCtrl::EditCtrl() {
}

char* const EditCtrl::registerWindowClass() {
	return EditCtrl::windowClassName_;
}

void EditCtrl::create(Window* parent, char* name, LONG style, DWORD exStyle) {
	if (style == 0) {
		style = WS_BORDER | ES_LEFT;
	}
	//style |= WS_CHILD | WS_VISIBLE;

	Ctrl::create(parent, name, style, exStyle);
}

EditCtrl::~EditCtrl() {
}

void EditCtrl::setText(TCHAR* text) {
	SYSPR(SetWindowText(hWnd_, text));
}

NS_FW_WIN_END