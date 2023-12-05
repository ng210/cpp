#include "test-ctrl.h"

WndClass TestCtrl::WndClass_;

TestCtrl::TestCtrl() {
	text_ = MALLOC(char, 1024);
	text_[0] = '\0';
}

TestCtrl::~TestCtrl() {
	FREE(text_);
}

WndClass TestCtrl::getWindowClass() {
	if (TestCtrl::WndClass_.atom == 0) {
		TestCtrl::WndClass_.atom = Window::registerClass("TestCtrl", NULL, 0);
	}
	return TestCtrl::WndClass_;
}

void TestCtrl::createWindow(Window* parent, char* name, LONG style, DWORD exStyle) {
	Ctrl::createWindow(parent, name, style, exStyle);
	hBrush_ = CreateSolidBrush(0x00000000);
}

void TestCtrl::setText(char* text) {
	strncpy_s(text_, 1024, text, 1024);
	InvalidateRect(hWnd_, NULL, true);
}

LRESULT TestCtrl::onCreate() {
	onLeftUp(TestCtrl::onLeftUpProc);
	return 0;
}

LRESULT TestCtrl::onPaint() {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd_, &ps);
	SetTextColor(hdc, COLOR_GRAYTEXT);
	//FillRect(hdc, &rect_, hBrush_);
	DrawText(hdc, text_, -1, &rect_, DT_CENTER);
	EndPaint(hWnd_, &ps);
	return 0;
}

LRESULT TestCtrl::onLeftUpProc(Window* wnd, POINT& pos, WPARAM state) {
	var ctrl = (TestCtrl*)wnd;
	ctrl->text_[0] = '\0';
	UpdateWindow(ctrl->hWnd_);
	ctrl->parent_->onLeftUp()(ctrl->parent_, pos, state);
	return 0;
}
