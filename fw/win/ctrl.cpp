#include "win/ctrl.h"

NS_FW_WIN_BEGIN

Ctrl::Ctrl() {
	ctrlId_ = 0;
	offset_.x = 0;
	offset_.y = 0;
}

void Ctrl::createWindow(Window* parent, char* name, LONG style, DWORD exStyle) {
	style |= WS_CHILD | WS_VISIBLE;
	this->Window::create(parent, name, style, exStyle);
}

void Ctrl::move(int x, int y) {
	SYSPR(SetWindowPos(hWnd_, NULL, x, y, 0, 0, SWP_NOSIZE));
	offset_.x = x;
	offset_.y = y;
}

NS_FW_WIN_END