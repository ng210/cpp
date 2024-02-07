#include "test-ctrl.h"

char* TestCtrl::windowClassName_ = "TestCtrl";
ATOM TestCtrl::windowClass_ = 0;

#define TESTCTRL_BUFFERSIZE 1048576

TestCtrl::TestCtrl() {
	text_ = MALLOC(char, TESTCTRL_BUFFERSIZE);
	text_[0] = '\0';
}

TestCtrl::~TestCtrl() {
	FREE(text_);
}

char* const TestCtrl::registerWindowClass() {
	if (TestCtrl::windowClass_ == 0) {
		TestCtrl::windowClass_ = Window::registerClass(TestCtrl::windowClassName_, NULL, 0);
	}
	return TestCtrl::windowClassName_;
}

void TestCtrl::create(Window* parent, char* name, LONG style, DWORD exStyle) {
	Ctrl::create(parent, name, style, exStyle);
	hBrush_ = CreateSolidBrush(0x00000000);
}

void TestCtrl::setText(char* text) {
	strncpy_s(text_, TESTCTRL_BUFFERSIZE, text, TESTCTRL_BUFFERSIZE);
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
