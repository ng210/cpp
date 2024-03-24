#ifndef __WINDOW_H
#define __WINDOW_H

#include <windows.h>
#include "basedef.h"
#include "base/memory.h"
#include "base/debug.h"
#include "collection/map.h"

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

#define SB_DELTA 16

//typedef union WndClass_ {
//	ATOM atom;
//	char* className;
//	WndClass_(char* s) { className = s; }
//	WndClass_(ATOM a) { className = (char*)a; }
//	WndClass_() { className = NULL; }
//} WndClass;

typedef struct ScrollInfo {
	int pos;
	int max;
	int lineSize;
	int pageSize;
} ScrollInfo;

class Window;
typedef LRESULT(MOUSEEVENTPROC)(Window*, POINT&, WPARAM);
typedef LRESULT(MOUSEMOVEEVENTPROC)(Window*, POINT&, POINT&, WPARAM);

class Window {
	static char* windowClassName_;

	static MOUSEMOVEEVENTPROC defOnMouseMoveProc_;
	static MOUSEEVENTPROC defOnMouseEventProc_;

	protected: WNDPROC defWindowProc_;
	protected: PROP_R(HINSTANCE, hInstance);
	protected: PROP_R(HWND, hWnd);
	protected: char name_[32];
	protected: PROP_R(RECT, rect);
	protected: PROP_R(RECT, virtualRect);
	protected: PROP_R(POINT, mousePos);
	//protected: PROP_R(WndClass, wndClass);
	protected: PROP_R(Window*, parent);

	protected: PROP_R(ScrollInfo, scrollInfoX);
	protected: PROP_R(ScrollInfo, scrollInfoY);

	protected: void setName(char* name);
	protected: virtual char* const registerWindowClass();
	protected: virtual int scrollWindow(ScrollInfo& scrollInfo, int mode, int pos, int nBar, bool update = true);
	protected: void updateScrolling();

	public:
		Window();
		virtual ~Window();
	
		void create(Window* parent, char* name, LONG style = NULL, DWORD exStyle = NULL);
		LONG setWindowStyle(LONG style, DWORD exStyle = 0);
		virtual LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		Map* Window::createChildWindowMap();
		const char* getName();
		
		LRESULT show(int cmdShow);
		LRESULT set(int x, int y, int width, int height, Window* insertAfter = NULL);
		LRESULT set(RECT& rect, Window* insertAfter = NULL);
		LRESULT setPosition(int x, int y);
		LRESULT setSize(int width, int height);
		virtual void Window::setVirtualSize(int width, int height);

	#pragma region EventHandling
		LRESULT onMouse(HWND, UINT, POINT&, WPARAM);

	#pragma region DefMouseEventProc
	protected: PROP(MOUSEMOVEEVENTPROC*, onMouseMove);
	protected: PROP(MOUSEEVENTPROC*, onLeftUp);
	protected: PROP(MOUSEEVENTPROC*, onLeftDown);
	protected: PROP(MOUSEEVENTPROC*, onLeftClick);
	protected: PROP(MOUSEEVENTPROC*, onLeftDblClick);
	protected: PROP(MOUSEEVENTPROC*, onRightUp);
	protected: PROP(MOUSEEVENTPROC*, onRightDown);
	protected: PROP(MOUSEEVENTPROC*, onRightClick);
	protected: PROP(MOUSEEVENTPROC*, onRightDblClick);
	protected: PROP(MOUSEEVENTPROC*, onMiddleUp);
	protected: PROP(MOUSEEVENTPROC*, onMiddleDown);
	protected: PROP(MOUSEEVENTPROC*, onMiddleClick);
	protected: PROP(MOUSEEVENTPROC*, onMiddleDblClick);
	protected: PROP(MOUSEEVENTPROC*, onWheel);			 
	#pragma endregion

	#pragma region SystemEvents
		virtual LRESULT onCreate();
		virtual LRESULT onCreated();
		virtual LRESULT onClose();
		virtual LRESULT onDestroy();
		virtual LRESULT onPaint();
		virtual LRESULT onEraseBkgnd(HDC hDC);
		virtual LRESULT onMove(POINT& point);
		virtual LRESULT onMoving(RECT& rect);
		virtual LRESULT onSize(RECT& rect, WPARAM state);
		virtual LRESULT onSizing(RECT& rect, WPARAM state);
		virtual LRESULT onCommand(WPARAM wParam, LPARAM lParam);
		virtual LRESULT onHScroll(WPARAM wParam, LPARAM lParam);
		virtual LRESULT onVScroll(WPARAM wParam, LPARAM lParam);
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