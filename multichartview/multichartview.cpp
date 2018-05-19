#include "MultiView.h"

NS_FW_BASE_USE

MultiView::MultiView(HWND hwnd, size_t wndId) {
	//instance_ = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
	instance_ = SYS(GetModuleHandle(NULL));
	if (wndClass_ == NULL) {
		registerClass();
	}
	create(hwnd, wndId);
	if (hwnd_) {
		SYS(ShowWindow(hwnd_, SW_SHOWNORMAL));
		SYS(UpdateWindow(hwnd_));
	}
	wndId_ = wndId;
	create(hwnd, wndId);
}

MultiView::~MultiView() {
}

HWND MultiView::create(HWND parent, size_t wndId) {
	ATOM wndClass = registerClass();
	hwnd_ = SYS(CreateWindowEx(
		0,
		MAKEINTATOM(wndClass), NULL,
		WS_CHILDWINDOW,
		CW_USEDEFAULT, 0,
		CW_USEDEFAULT, 0,
		parent,
		(HMENU)wndId,
		instance_,
		this));

	return hwnd_;
}

ATOM MultiView::registerClass() {
	MultiView::wndClass_ = WindowWrapper::registerClassByName(L"MultiView");
	return MultiView::wndClass_;
}

LRESULT MultiView::onCreate() {
	return 0;
}
LRESULT MultiView::onCommand(int wmId) {
	return 0;
}
LRESULT MultiView::onPaint(HDC hdc, PAINTSTRUCT* ps) {
	TextOut(hdc, 0, 0, L"MultiView", 15);
	return 0;
}
LRESULT MultiView::onDestroy() {
	return 0;
}


void MultiView::setPosition(int x, int y) {
	SYS(SetWindowPos(hwnd_, NULL, x, y, 0, 0, SWP_NOSIZE || SWP_SHOWWINDOW));
}

void MultiView::setSize(int width, int height) {
	SYS(SetWindowPos(hwnd_, NULL, 0, 0, width, height, SWP_NOMOVE || SWP_SHOWWINDOW));
}

LRESULT MultiView::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	return WindowWrapper::wndProc(hWnd, message, wParam, lParam);;
}

