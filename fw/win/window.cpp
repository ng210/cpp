#include <windowsx.h>
#include "win/window.h"
#include "base/debug.h"

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

static int wm_sizing_wparam[] = {
	0x00,	// NONE
	SIZING_LEFT,
	SIZING_RIGHT,
	SIZING_TOP,
	SIZING_LEFT | SIZING_TOP,
	SIZING_RIGHT | SIZING_TOP,
	SIZING_BOTTOM,
	SIZING_LEFT | SIZING_BOTTOM,
	SIZING_RIGHT | SIZING_BOTTOM
};

char* Window::windowClassName_ = "Window";

Window::Window() {
	hWnd_ = NULL;
	parent_ = NULL;
	hInstance_ = NULL;
	defWindowProc_ = DefWindowProc;
	mousePos_ = { 0, 0 };
	rect_ = { 0, 0, 0, 0 };
	virtualRect_ = { 0, 0, 0, 0 };
	fmw::memset(&scrollInfoX_, 0, 2 * sizeof(SCROLLINFO));
	scrollInfoX_.lineSize = 1;
	scrollInfoY_.lineSize = 1;
	scrollInfoX_.pageSize = 10;
	scrollInfoY_.pageSize = 10;
}

Window::~Window() {
	if (hWnd_) {
		if (!parent_) {
			SYSPR(SendMessage(hWnd_, WM_CLOSE, NULL, NULL));
		}
		else {
			onClose();
			SYSPR(DestroyWindow(hWnd_));
		}
	}
}

void Window::create(Window* parent, char* name, LONG style, DWORD exStyle) {
	HWND hWndParent = NULL;
	if (parent) {
		parent_ = parent;
		hInstance_ = parent->hInstance();
		hWndParent = parent->hWnd();
	}
	else {
		SYSFN(hInstance_, GetModuleHandle(NULL));
	}

	// create window
	var className = registerWindowClass();
	if (style == 0) style = Window::defaultStyle;
	if (hWnd_ == NULL) {
		SYSFN(hWnd_, CreateWindowEx(exStyle, className, name, style, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWndParent, NULL, hInstance_, this));
	}
	if (hWnd_ != NULL) {
		// check wrapper function
		WNDCLASSEX wndClassEx;
		SYSPR(GetClassInfoEx(hInstance_, className, &wndClassEx));
		if (wndClassEx.lpfnWndProc != Window::wndProcWrapper) {
			defWindowProc_ = wndClassEx.lpfnWndProc;
			SetLastError(0);
			SYSPR(SetWindowLongPtr(hWnd_, GWLP_WNDPROC, (LONG_PTR)Window::wndProcWrapper));
		}
		SetLastError(0);
		SYSPR(SetWindowLongPtr(hWnd_, GWLP_USERDATA, (LONG_PTR)this));

		// set default event handlers
		MOUSEEVENTPROC** p = &onLeftUp_;
		while (p <= &onWheel_) *p++ = &defOnMouseEventProc_;
		onMouseMove_ = &defOnMouseMoveProc_;
	}

	this->onCreated();
}

LONG Window::setWindowStyle(LONG style, DWORD exStyle) {
	LONG SYSFN(result, SetWindowLong(hWnd_, GWL_STYLE, style));
	SetWindowLong(hWnd_, GWL_STYLE, style);
	if (result != 0 && exStyle != 0) {
		SYSFN(result, SetWindowLong(hWnd_, GWL_EXSTYLE, exStyle));
	}
	return result;
}

char* const Window::registerWindowClass() {
	return Window::windowClassName_;
}

//void Window::sizing(int sizingEdge, LPRECT sizingRect, LPRECT confineRect, int width, int height) {
//	int direction = wm_sizing_wparam[sizingEdge];
//	if (confineRect != NULL) {
//		if (sizingRect->left < confineRect->left) sizingRect->left = confineRect->left;
//		if (sizingRect->right < confineRect->right) sizingRect->right = confineRect->right;
//		if (sizingRect->top < confineRect->top) sizingRect->top = confineRect->top;
//		if (sizingRect->bottom < confineRect->bottom) sizingRect->bottom = confineRect->bottom;
//	}
//	if (sizingRect->right - sizingRect->left < width) {
//		if (direction & SIZING_LEFT) {
//			sizingRect->left = sizingRect->right - width;
//		} else if (direction & SIZING_RIGHT) {
//			sizingRect->right = sizingRect->left + width;
//		}
//	}
//	if (sizingRect->bottom - sizingRect->top < height) {
//		if (direction & SIZING_TOP) {
//			sizingRect->top = sizingRect->bottom - height;
//		} else if (direction & SIZING_BOTTOM) {
//			sizingRect->bottom = sizingRect->top + height;
//		}
//	}
//}

LRESULT CALLBACK Window::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT ret = 1;
	BOOL callDefault = true;
	POINT point;
	if (message >= WM_MOUSEMOVE && message <= WM_MOUSEWHEEL) {
		point.x = GET_X_LPARAM(lParam);
		point.y = GET_Y_LPARAM(lParam);
		ret = onMouse(hWnd, message, point, wParam);
	}
	else {
		switch (message) {
		case WM_CREATE:
			ret = onCreate();
			break;
		case WM_CLOSE:
			ret = onClose();
			DestroyWindow(hWnd_);
			break;
		case WM_DESTROY:
			ret = onDestroy();
			if (parent_ == NULL) {
				PostQuitMessage((int)ret);
			}
			hWnd_ = NULL;
			callDefault = ret != 0;
			break;
		case WM_MOVE:
			point.x = LOWORD(lParam);
			point.y = HIWORD(lParam);
			ret = onMove(point);
			break;
		case WM_MOVING:
			ret = onMoving(*(RECT*)lParam);
			callDefault = ret != 0;
			break;
		case WM_SIZE:
			RECT rect;
			rect.right = LOWORD(lParam), rect.bottom = HIWORD(lParam);
			ret = onSize(rect, wParam);
			rect_.right = rect.right;
			rect_.bottom = rect.bottom;
			updateScrolling();
			callDefault = ret != 0;
			break;
		case WM_SIZING:
			ret = onSizing(*(RECT*)lParam, wParam);
			callDefault = !ret;
			break;
		case WM_PAINT:
			ret = onPaint();
			callDefault = ret != 0;
			break;
		case WM_ERASEBKGND:
			ret = onEraseBkgnd((HDC)wParam);
			callDefault = ret == 0;
			break;			
		case WM_COMMAND:
			ret = onCommand(wParam, lParam);
			callDefault = ret != 0;
			break;

		case WM_HSCROLL:
			ret = onHScroll(wParam, lParam);
			break;
		case WM_VSCROLL:
			ret = onVScroll(wParam, lParam);
			break;
		}
	}

	if (callDefault) {
		ret = defWindowProc_(hWnd, message, wParam, lParam);
	}

	//if (parent_ != NULL) {
	//	parent_->wndProc(hWnd, message, wParam, lParam);
	//}

	return ret;
}

BOOL enumChildWindowsCallback(HWND hWnd, LPARAM lParam) {
	var map = (Map*)lParam;
	var SYSFN(id, GetDlgCtrlID(hWnd));
	map->add(&id, &hWnd);
	return true;
}

Map* Window::createChildWindowMap() {
	var map = NEW_(Map, sizeof(int), sizeof(HWND), Map::hashingInt, Collection::compareInt);
	SYSPR(EnumChildWindows(hWnd_, enumChildWindowsCallback, (LPARAM)map));
	return map;
}

LRESULT Window::show(int cmdShow) {
	return ShowWindow(hWnd_, cmdShow);
}

LRESULT Window::set(int x, int y, int width, int height, Window* insertAfter) {
	LRESULT result = 0;
	var hWnd = insertAfter ? insertAfter->hWnd_ : NULL;
	int flags = 0;
	if (x == -1) flags |= SWP_NOMOVE;
	if (width == 0) flags |= SWP_NOSIZE;
	if (insertAfter == NULL) flags |= SWP_NOZORDER;
	SYSFN(result, SetWindowPos(hWnd_, hWnd, x, y, width, height, flags));
	return result;
}

LRESULT Window::set(RECT& rect, Window* insertAfter) {
	return set(rect.left, rect.top, rect.right, rect.bottom, insertAfter);
}

LRESULT Window::setPosition(int x, int y) {
	LRESULT result = 0;
	SYSFN(result, SetWindowPos(hWnd_, NULL, x, y, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE));
	return result;
}

LRESULT Window::setSize(int width, int height) {
	LRESULT result = 0;
	SYSFN(result, SetWindowPos(hWnd_, NULL, 0, 0, width, height, SWP_SHOWWINDOW | SWP_NOMOVE));
	return result;
}

void Window::setVirtualSize(int width, int height) {
	virtualRect_.right = width;
	virtualRect_.bottom = height;
	updateScrolling();
	InvalidateRect(hWnd_, NULL, false);
}

void Window::updateScrolling() {
	SCROLLINFO scrollInfo;
	
	scrollInfoX_.max = max(virtualRect_.right - rect_.right, 0);
	if (scrollInfoX_.max > 0) {
		scrollInfo.cbSize = sizeof(SCROLLINFO);
		scrollInfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
		scrollInfo.nMin = 0;
		scrollInfo.nMax = virtualRect_.right;
		scrollInfo.nPage = rect_.right;
		scrollInfo.nPos = scrollInfoX_.pos;
		SYSPR(SetScrollInfo(hWnd_, SB_HORZ, &scrollInfo, true));
	}

	scrollInfoY_.max = max(virtualRect_.bottom - rect_.bottom, 0);
	if (scrollInfoY_.max > 0) {
		scrollInfo.cbSize = sizeof(SCROLLINFO);
		scrollInfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
		scrollInfo.nMin = 0;
		scrollInfo.nMax = virtualRect_.bottom;
		scrollInfo.nPage = rect_.bottom;
		scrollInfo.nPos = scrollInfoY_.pos;
		SYSPR(SetScrollInfo(hWnd_, SB_VERT, &scrollInfo, true));
	}
}

int Window::scrollWindow(ScrollInfo& si, int mode, int pos, int nBar, bool update) {
	int newPos = 0;
	int oldPos = si.pos;
	switch (mode)
	{
	case SB_PAGEUP:
		newPos = oldPos - si.pageSize;
		break;
	case SB_PAGEDOWN:
		newPos = oldPos + si.pageSize;
		break;
	case SB_LINEUP:
		newPos = oldPos - si.lineSize;
		break;
	case SB_LINEDOWN:
		newPos = oldPos + si.lineSize;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		newPos = pos;
		break;
	case SB_DELTA:
		newPos = oldPos + pos;
		break;
	default:
		newPos = si.pos;
	}

	if (newPos < 0) newPos = 0;
	else if (newPos > si.max) newPos = si.max;

	if (si.pos != newPos) {
		si.pos = newPos;
		SCROLLINFO scrollInfo;
		scrollInfo.cbSize = sizeof(SCROLLINFO);
		scrollInfo.fMask = SIF_POS;
		scrollInfo.nPos = newPos;
		SYSPR(SetScrollInfo(hWnd_, nBar, &scrollInfo, TRUE));
		SYSPR(InvalidateRect(hWnd_, NULL, false));
	}

	return oldPos - newPos;
}

#pragma region EventHandling

#pragma region MouseEvents
LRESULT Window::onMouse(HWND hWnd, UINT message, POINT& point, WPARAM wParam) {
	LRESULT ret = 0;
	POINT delta;
	switch (message) {
	case WM_MOUSEMOVE:
		delta.x = point.x - mousePos_.x;
		delta.y = point.y - mousePos_.y;
		ret = onMouseMove_(this, point, delta, wParam);
		mousePos_ = point;
		break;
	case WM_LBUTTONDOWN:
		ret = onLeftDown_(this, point, wParam);
		break;
	case WM_LBUTTONUP:
		ret = onLeftUp_(this, point, wParam);
		if (!ret) {
			ret = onLeftClick_(this, point, wParam);
		}
		break;
	case WM_LBUTTONDBLCLK:
		ret = onLeftDblClick_(this, point, wParam);
		break;
	case WM_RBUTTONDOWN:
		ret = onRightDown_(this, point, wParam);
		break;
	case WM_RBUTTONUP:
		ret = onRightUp_(this, point, wParam);
		if (!ret) {
			ret = onRightClick_(this, point, wParam);
		}
		break;
	case WM_RBUTTONDBLCLK:
		ret = onRightDblClick_(this, point, wParam);
		break;
	case WM_MBUTTONDOWN:
		ret = onMiddleDown_(this, point, wParam);
		break;
	case WM_MBUTTONUP:
		ret = onMiddleUp_(this, point, wParam);
		if (!ret) {
			ret = onMiddleClick_(this, point, wParam);
		}
		break;
	case WM_MBUTTONDBLCLK:
		ret = onMiddleDblClick_(this, point, wParam);
		break;
	case WM_MOUSEWHEEL:
		ret = onWheel_(this, point, wParam);
		break;
	}
	return ret;
}
LRESULT Window::defOnMouseEventProc_(Window*, POINT&, WPARAM) {
	return 1;
}
LRESULT Window::defOnMouseMoveProc_(Window*, POINT&, POINT&, WPARAM) {
	return 1;
}
#pragma endregion

#pragma region SystemEvents
LRESULT Window::onCreate() {
	return 0;
}
LRESULT Window::onCreated() {
	return 0;
}
LRESULT Window::onClose() {
	return 0;
}
LRESULT Window::onDestroy() {
	return 0;
}
LRESULT Window::onPaint() {
	return 1;
}
LRESULT Window::onEraseBkgnd(HDC hDC) {
	return 0;
}
LRESULT Window::onMove(POINT& point) { 
	return 1;
}
LRESULT Window::onMoving(RECT& rect) { 
	return 1;
}
LRESULT Window::onSize(RECT& rect, WPARAM state) {
	updateScrolling();
	return 1;
}
LRESULT Window::onSizing(RECT& rect, WPARAM state) {
	return 1;
}
LRESULT Window::onHScroll(WPARAM wParam, LPARAM lParam) {
	scrollWindow(scrollInfoX_, LOWORD(wParam), HIWORD(wParam), SB_HORZ);
	return 1;
}
LRESULT Window::onVScroll(WPARAM wParam, LPARAM lParam) {
	scrollWindow(scrollInfoY_, LOWORD(wParam), HIWORD(wParam), SB_VERT);
	return 1;
}
LRESULT Window::onCommand(WPARAM wParam, LPARAM lParam) {
	return 1;
}
#pragma endregion

#pragma endregion

#pragma region Statics
LRESULT CALLBACK Window::wndProcWrapper(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT ret = 0;
	LONG_PTR lPtr = NULL;
	Window* window = NULL;

	if (message == WM_CREATE) {
		var cs = (CREATESTRUCT*)lParam;
		window = (Window*)cs->lpCreateParams;
		ret = 1;
	} else {
		LONG_PTR SYSFN(lptr, GetWindowLongPtr(hWnd, GWLP_USERDATA));
		window = (Window*)lptr;
	}

	if (window != NULL) {
		ret = (window->wndProc)(hWnd, message, wParam, lParam);
	}
	else {
		SYSFN(ret, DefWindowProc(hWnd, message, wParam, lParam));
	}
	return ret;
}

LONG Window::defaultStyle = WS_OVERLAPPEDWINDOW;

ATOM Window::registerClass(WNDCLASSEX& wndClassEx) {
	wndClassEx.lpfnWndProc = (WNDPROC)&Window::wndProcWrapper;
	wndClassEx.cbWndExtra = sizeof(Window*);
	wndClassEx.hInstance = (HINSTANCE)GetModuleHandle(NULL);
	ATOM SYSFN(wndClass, RegisterClassEx(&wndClassEx));
	wndClass &= 0x0000FFFF;
	return wndClass;
}

ATOM Window::registerClass(char* name, HINSTANCE hInstance, UINT style) {
	//if (style == 0) {
	//	style = CS_HREDRAW | CS_VREDRAW;
	//}
	WNDCLASSEX wndClassEx = {
		sizeof(WNDCLASSEX),				// cbSize;
		style,							// style;
		& Window::wndProcWrapper,		// lpfnWndProc;
		0,                              // cbClsExtra;
		0,                              // cbWndExtra;
		hInstance,                      // hInstance;
		NULL,                           // hIcon;
		LoadCursor(NULL, IDC_ARROW),    // hCursor;
		(HBRUSH)COLOR_BACKGROUND,		// hbrBackground;
		NULL,                           // lpszMenuName;
		name,                           // lpszClassName;
		NULL                            // hIconSm;
	};

	return Window::registerClass(wndClassEx);
}

#pragma endregion

NS_FW_WIN_END