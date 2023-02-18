#ifndef __WINDOW_H
#define __WINDOW_H

#include <windows.h>
#include "basedef.h"
#include "base/memory.h"
#include "base/debug.h"

//*********************************************************
#define NS_FW_WIN fw_win
#define NS_FW_WIN_BEGIN namespace NS_FW_WIN {
#define NS_FW_WIN_END }
#define NS_FW_WIN_USE using namespace NS_FW_WIN;

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

#define SIZING_LEFT		1
#define SIZING_RIGHT	2
#define SIZING_TOP		4
#define SIZING_BOTTOM	8

typedef union WndClass_ {
	ATOM atom;
	char* className;
	WndClass_(char* s) { className = s; }
	WndClass_(ATOM a) { className = (char*)a; }
	WndClass_() { className = NULL; }
} WndClass;

class Window {
	protected: WNDPROC defWindowProc_;
	protected: PROP_R(HINSTANCE, hInstance);
	protected: PROP_R(HWND, hWnd);
	protected: PROP_R(RECT, rect);
	protected: PROP_R(POINT, mousePos);
	protected: PROP_R(WndClass, wndClass);
	protected: PROP_R(Window*, parent);
	//protected: PROP_R(bool, propagateMessage);
public:
	Window();
	virtual ~Window();

	virtual void create(WndClass wndClass, Window* parent, char* name, LONG style = NULL, DWORD exStyle = NULL);
	LONG setWindowStyle(LONG style, DWORD exStyle = 0);
	virtual LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#pragma region EventHandling

#pragma region MouseEvents
	virtual LRESULT onMouse(HWND hWnd, UINT message, POINT& point, WPARAM wParam);
	virtual LRESULT onLeftUp(POINT& pos, WPARAM state);
	virtual LRESULT onLeftDown(POINT& pos, WPARAM state);
	virtual LRESULT onLeftClick(POINT& pos, WPARAM state);
	virtual LRESULT onLeftDblClick(POINT& pos, WPARAM state);
	virtual LRESULT onRightUp(POINT& pos, WPARAM state);
	virtual LRESULT onRightDown(POINT& pos, WPARAM state);
	virtual LRESULT onRightClick(POINT& pos, WPARAM state);
	virtual LRESULT onRightDblClick(POINT& pos, WPARAM state);
	virtual LRESULT onMiddleUp(POINT& pos, WPARAM state);
	virtual LRESULT onMiddleDown(POINT& pos, WPARAM state);
	virtual LRESULT onMiddleClick(POINT& pos, WPARAM state);
	virtual LRESULT onMiddleDblClick(POINT& pos, WPARAM state);
	virtual LRESULT onMouseMove(POINT& pos, POINT& delta, WPARAM state);
#pragma endregion

#pragma region SystemEvents
	virtual LRESULT onCreate();
	virtual LRESULT onDestroy();
	virtual LRESULT onPaint();
	virtual LRESULT onMove(POINT& point);
	virtual LRESULT onMoving(RECT& rect);
	virtual LRESULT onSize(RECT& rect, WPARAM state);
	virtual LRESULT onSizing(RECT& rect, WPARAM state);
	virtual LRESULT onCommand(WPARAM wParam, LPARAM lParam);
#pragma endregion

#pragma endregion

#pragma region Statics
protected:
	static LRESULT CALLBACK wndProcWrapper(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	static LONG defaultStyle;
	static ATOM registerClass(WNDCLASSEX& wndClassEx);
	static ATOM registerClass(char* name, HINSTANCE hInstance, UINT style = 0);
#pragma endregion

	//static void getCreateStructures(CREATESTRUCT& createStruct, WNDCLASSEX& wndClassEx, HINSTANCE hInstance, const char* appName, const char* wClassName);
};

NS_FW_WIN_END

#endif