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

Window::Window() {
	hWnd_ = NULL;
	parent_ = NULL;
	hInstance_ = NULL;
	defWindowProc_ = DefWindowProc;
	mousePos_ = { 0, 0 };
	rect_ = { 0, 0, 0, 0 };
}

void Window::create(WndClass wndClass, Window* parent, char* name, LONG style, DWORD exStyle) {
	wndClass_ = wndClass;
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
	if (style == 0) style = Window::defaultStyle;
	SYSFN(hWnd_, CreateWindowEx(exStyle, wndClass.className, name, style, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWndParent, NULL, hInstance_, this));

	if (hWnd_ != NULL) {
		// check wrapper function
		WNDCLASSEX wndClassEx;
		SYSPR(GetClassInfoEx(hInstance_, wndClass.className, &wndClassEx));
		if (wndClassEx.lpfnWndProc != Window::wndProcWrapper) {
			defWindowProc_ = wndClassEx.lpfnWndProc;
			SetLastError(0);
			SYSPR(SetWindowLongPtr(hWnd_, GWLP_WNDPROC, (LONG_PTR)Window::wndProcWrapper));
		}
		SetLastError(0);
		SYSPR(SetWindowLongPtr(hWnd_, GWLP_USERDATA, (LONG_PTR)this));

		// set default event handlers
		MOUSEEVENTPROC** p = &onLeftUp_;
		while (p <= &onMiddleDblClick_) *p++ = &defOnMouseEventProc_;
		onMouseMove_ = &defOnMouseMoveProc_;

		this->onCreate();
	}
}

Window::~Window() {
}

LONG Window::setWindowStyle(LONG style, DWORD exStyle) {
	LONG SYSFN(result, SetWindowLong(hWnd_, GWL_STYLE, style));
	SetWindowLong(hWnd_, GWL_STYLE, style);
	if (result != 0 && exStyle != 0) {
		SYSFN(result, SetWindowLong(hWnd_, GWL_EXSTYLE, exStyle));
	}
	return result;
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
	POINT point;
	if (message >= WM_MOUSEMOVE && message <= WM_MBUTTONDBLCLK) {
		point.x = LOWORD(lParam);
		point.y = HIWORD(lParam);
		ret = onMouse(hWnd, message, point, wParam);
	}
	else {
		switch (message) {
		//case WM_CREATE:
		//	ret = ret / 0;
		//	break;
		case WM_DESTROY:
			ret = onDestroy();
			PostQuitMessage(0);
			ret = 1;
			break;
		case WM_MOVE:
			point.x = LOWORD(lParam);
			point.y = HIWORD(lParam);
			ret = onMove(point);
			break;
		case WM_MOVING:
			ret = onMoving(*(RECT*)lParam);
			break;
		case WM_SIZE:
			rect_.right = LOWORD(lParam);
			rect_.bottom = HIWORD(lParam);
			ret = onSize(rect_, wParam);
			break;
		case WM_SIZING:
			ret = onSizing(*(RECT*)lParam, wParam);
			break;
		case WM_PAINT:
			ret = onPaint();
			break;
		case WM_COMMAND:
			ret = onCommand(wParam, lParam);
			break;
		}
	}

	if (ret) {
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
LRESULT Window::onDestroy() {
	return 0;
}
LRESULT Window::onPaint() {
	return 1;
}
LRESULT Window::onMove(POINT& point) { 
	return 1;
}
LRESULT Window::onMoving(RECT& rect) { 
	return 1;
}
LRESULT Window::onSize(RECT& rect, WPARAM state) { 
	return 1;
}
LRESULT Window::onSizing(RECT& rect, WPARAM state) {
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
	//if (message == WM_NCCREATE || message == WM_CREATE) {
	//	window = *(Window**)lParam;
	//	SYSPR(SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)window));
	//	window->hWnd_ = hWnd;
	//}
	//else {
		LONG_PTR SYSFN(lptr, GetWindowLongPtr(hWnd, GWLP_USERDATA));
		window = (Window*)lptr;
	//}
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
	if (style == 0) {
		style = CS_HREDRAW | CS_VREDRAW;
	}
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