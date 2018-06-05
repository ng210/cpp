#ifndef __WINDOW_H
#define __WINDOW_H

#include <windows.h>
#include "basedef.h"
#include "base/memory.h"
#include "base/debug.h"

//*********************************************************
#define NS_FW_WIN win
#define NS_FW_WIN_BEGIN namespace NS_FW_WIN {
#define NS_FW_WIN_END }
#define NS_FW_WIN_USE using namespace NS_FW_WIN;

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

#define SIZING_LEFT		1
#define SIZING_RIGHT	2
#define SIZING_TOP		4
#define SIZING_BOTTOM	8

typedef union WINDOWCLASS_ {
	ATOM atom;
	LPTSTR str;
	size_t v;
	WINDOWCLASS_() : v(0) {}
} WINDOWCLASS;

class Window {
	protected: PROP_R(HINSTANCE, hInstance);
	protected: PROP_R(HWND, hWnd);
	protected: PROP_R(RECT, rect);
	protected: PROP_R(int, width);
	protected: PROP_R(int, height);
protected:
	WINDOWCLASS wndClass_;

	Window() {};
	static LRESULT CALLBACK wndProcWrapper(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	Window(CREATESTRUCT* createStruct, Window* parent = NULL, WNDCLASSEX* wndClassEx = NULL);
	virtual ~Window();

	inline WINDOWCLASS wndClass() { return wndClass_; }

	HWND create(CREATESTRUCT* createStruct, Window* parent = NULL, WNDCLASSEX* wndClassEx = NULL);
	//void sizing(int sizingEdge, LPRECT sizingRect, LPRECT confineRect, int width, int height);
	virtual LRESULT onCreate();
	//virtual int onMove(int x, int y);
	virtual LRESULT onSize(WPARAM wParam, LPARAM lParam);
	virtual LRESULT onCommand(WPARAM wParam, LPARAM lParam);
	//virtual int onMouseClick(POINT, int);
	//virtual int onMouseMove(POINT);
	virtual int onPaint(HDC hdc, PAINTSTRUCT* ps);
	virtual int onDestroy();
};

NS_FW_WIN_END

#endif