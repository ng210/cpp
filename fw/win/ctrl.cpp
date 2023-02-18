#include "win/ctrl.h"

NS_FW_WIN_BEGIN

Ctrl::Ctrl() {
	ctrlId_ = 0;
}

void Ctrl::create(WndClass wndClass, Window* parent, char* name, LONG style, DWORD exStyle) {
	style |= WS_CHILD | WS_VISIBLE;
	this->Window::create(wndClass, parent, name, style, exStyle);
}


NS_FW_WIN_END