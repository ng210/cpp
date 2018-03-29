#ifndef __WINDOWWRAPPER_H
#define __WINDOWWRAPPER_H

#include <windows.h>
#include "base/base_defs.h"
#include "base/MemoryMgr.h"

NS_FW_BASE_BEGIN

class WindowWrapper {
protected: PROP_R(HINSTANCE, instance);
protected: PROP_R(HWND, hwnd);
protected:
	ATOM registerClassByName(LPCTSTR className);
	void createWindow(ATOM wndClass, HWND parent = NULL, HMENU hmenu = NULL, int style = WS_OVERLAPPEDWINDOW);
	virtual LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK wndProcWrapper(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	WindowWrapper();
	//WindowWrapper(HINSTANCE instance);
	~WindowWrapper();

	virtual LRESULT onCreate();
	virtual LRESULT onCommand(int wmId);
	virtual LRESULT onPaint(HDC hdc, PAINTSTRUCT* ps);
	virtual LRESULT onDestroy();
};

NS_FW_BASE_END

#endif