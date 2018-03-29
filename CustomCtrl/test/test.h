#pragma once

#include "resource.h"
#include "winapp.h"
#include "MultiView.h"

#define MYCONTROLID 0x1234

NS_FW_BASE_USE

class TestApp : public WinApp {
	MultiView* multiView;
	ATOM wndClass_;
public:
	TestApp(HINSTANCE instance);
	~TestApp();

	LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	LRESULT onCreate();
	LRESULT onPaint(HDC hdc, PAINTSTRUCT* ps);
};