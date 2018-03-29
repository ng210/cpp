#include "test.h"

extern int winMain(HINSTANCE instance, Map* args) {
	TestApp testApp(instance);
	return testApp.main(NULL);
}

//int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow) {
//	return 0;
//}

TestApp::TestApp(HINSTANCE instance) {
	instance_ = SYS(GetModuleHandle(NULL));
	wndClass_ = WindowWrapper::registerClassByName(LPCWSTR("TestApp"));
	WindowWrapper::createWindow(wndClass_);
	//create(wndClass);
	if (hwnd_) {
		SYS(ShowWindow(hwnd_, SW_SHOWNORMAL));
		SYS(UpdateWindow(hwnd_));
	}
}

TestApp::~TestApp() {
	//DEL_(multiView);
}

LRESULT CALLBACK TestApp::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_SIZING: {
		RECT* rect;
		rect = (RECT*)lParam;
		int width = rect->right - rect->left;
		int height = rect->bottom - rect->top;
		if (height < 200) {
			switch (wParam) {
				case WMSZ_BOTTOM:
				case WMSZ_BOTTOMLEFT:
				case WMSZ_BOTTOMRIGHT:
					rect->bottom = rect->top + 200;
					break;
				case WMSZ_TOP:
				case WMSZ_TOPLEFT:
				case WMSZ_TOPRIGHT:
					rect->top = rect->bottom - 200;
					break;
				}
		}
		if (width < 320) {
			switch (wParam) {
				case WMSZ_RIGHT:
				case WMSZ_TOPRIGHT:
				case WMSZ_BOTTOMRIGHT:
					rect->right = rect->left + 320;
					break;
				case WMSZ_LEFT:
				case WMSZ_TOPLEFT:
				case WMSZ_BOTTOMLEFT:
					rect->left = rect->right - 320;
					break;
				}
			}
		}
		break;
	}
	return WinApp::wndProc(hWnd, message, wParam, lParam);
}

LRESULT TestApp::onCreate() {
	SYS(SetWindowText(hwnd_, L"Custom control test"));
	//multiView = NEW_(MultiView, hwnd_, MYCONTROLID);
	//multiView->create(hwnd_, MYCONTROLID);
	//multiView->setPosition(0, 0);
	//multiView->setSize(320, 200);
	//SYS(ShowWindow(multiView->hwnd(), SW_SHOWNORMAL));
	//SYS(UpdateWindow(multiView->hwnd()));
	return 0;
}

LRESULT TestApp::onPaint(HDC hdc, PAINTSTRUCT* ps) {
	RECT rect;
	POINT point;
	SYS(GetClientRect(hwnd_, &rect));
	SYS(FillRect(hdc, &rect, (HBRUSH)COLOR_APPWORKSPACE));
	for (int j = 32; j < rect.bottom; j += 24) {
		SYS(MoveToEx(hdc, 0, j, &point));
		SYS(LineTo(hdc, rect.right, j));
	}
	for (int i = 0; i < rect.right; i+=24) {
		SYS(MoveToEx(hdc, i, 32, &point));
		SYS(LineTo(hdc, i, rect.bottom));
	}
	return 0;
}
