#ifndef __TESTCTRL
#define __TESTCTRL

#include "base/WindowWrapper.h"

NS_FW_BASE_USE

class MultiView : public WindowWrapper {
	ATOM wndClass_;
protected: PROP_R(size_t, wndId);
protected:
	ATOM registerClass();
	HWND create(HWND parent, size_t wndId);

	LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT onCreate();
	LRESULT onCommand(int wmId);
	LRESULT onPaint(HDC hdc, PAINTSTRUCT* ps);
	LRESULT onDestroy();

public:
	MultiView(HWND hwnd, size_t wndId);
	virtual ~MultiView();
	void setPosition(int x, int y);
	void setSize(int width, int height);
};

#endif

