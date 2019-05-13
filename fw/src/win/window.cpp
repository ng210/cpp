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

Window::Window(CREATESTRUCT* createStruct, Window* parent, WNDCLASSEX* wndClassEx) {
	create(createStruct, parent, wndClassEx);
}

HWND Window::create(CREATESTRUCT* createStruct, Window* parent, WNDCLASSEX* wndClassEx) {
	if (createStruct->hInstance == NULL) {
		// instance is taken from parent or from module
		hInstance_ = parent == NULL ? GetModuleHandle(NULL) : parent->hInstance();
	} else {
		// instance was passed in
		hInstance_ = createStruct->hInstance;
	}
	// register class
	if (wndClassEx != NULL) {
		wndClassEx->lpfnWndProc = (WNDPROC)&Window::wndProcWrapper;
		wndClassEx->cbWndExtra = sizeof(Window*);
		wndClassEx->hInstance = hInstance_;
		SYSFN(wndClass_.atom, RegisterClassEx(wndClassEx));
		wndClass_.v &= 0x0000FFFF;
	} else {
		wndClass_.str = (LPTSTR)createStruct->lpszClass;
	}
	// create window
	hWnd_ = NULL;
	HWND SYSFN(hWnd, CreateWindowEx(createStruct->dwExStyle, wndClass_.str, createStruct->lpszName,
		createStruct->style, createStruct->x, createStruct->y, createStruct->cx, createStruct->cy,
		createStruct->hwndParent, createStruct->hMenu, hInstance_,
		this));
	return hWnd;
}

Window::~Window() {
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
//
//ATOM WindowWrapper::registerClassByName(LPCTSTR className) {
//	// register window class
//	WNDCLASSEX wndClassEx = {
//		sizeof(WNDCLASSEX),
//		CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
//		&WindowWrapper::wndProcWrapper,
//		0,
//		sizeof(WindowWrapper*),
//		instance_,
//		NULL,
//		LoadCursor(NULL, IDC_ARROW),
//		(HBRUSH)(BLACK_BRUSH + 1),
//		NULL,
//		className,
//		NULL
//	};
//	return SYS(RegisterClassEx(&wndClassEx));
//}

LRESULT CALLBACK Window::wndProcWrapper(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT ret = 0;
	LONG_PTR lPtr = NULL;
	Window* window = NULL;
	if (message == WM_NCCREATE || message == WM_CREATE) {
		window = *(Window**)lParam;
		if (window->hWnd_ == NULL) {
			window->hWnd_ = hWnd;
			SYSPR(SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)window));
		}
	} else {
		LONG_PTR SYSFN(lptr, GetWindowLongPtr(hWnd, GWLP_USERDATA));
		window = (Window*)lptr;
	}
	if (window != NULL) {
		ret = (window->wndProc)(hWnd, message, wParam, lParam);
	} else {
		SYSFN(ret, DefWindowProc(hWnd, message, wParam, lParam));
	}
	return ret;
}
LRESULT CALLBACK Window::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT ret = 0;
	switch (message) {
	case WM_CREATE:
		onCreate();
		break;
	case WM_DESTROY:
		onDestroy();
		PostQuitMessage(0);
		ret = 1;
		break;
	case WM_COMMAND:
		ret = onCommand(wParam, lParam);
		break;
	case WM_PAINT:
		PAINTSTRUCT ps;
		HDC hdc;
		SYSFN(hdc, BeginPaint(hWnd, &ps));
		ret = onPaint(hdc, &ps);
		SYSPR(EndPaint(hWnd, &ps));
		break;
	case WM_SIZE:
		width_ = LOWORD(lParam); height_ = HIWORD(lParam);
		ret = onSize(wParam, lParam);
		break;
	//case WM_COMMAND:
	//	onCommand(LOWORD(wParam));
	//	break;
	//case WM_SIZING:
	//	//onSizing(wParam, (RECT*)lParam);
	//	//RECT* rect;
	//	//rect = (RECT*)lParam;
	//	//LOG("(%04d, %04d) (%04d, %04d)\n", rect->left, rect->top, rect->right, rect->bottom);
	//	break;
	default:
		ret = DefWindowProc(hWnd, message, wParam, lParam);
	}
	//if (ret == 0) {
	//	//ret = DefWindowProc(hWnd, message, wParam, lParam);
	//}
	return ret;
}

LRESULT Window::onCreate() {
	return 0;
}
//int Window::onMove(int x, int y) {
//	SYSPR(SetWindowPos(hWnd_, NULL, x, y, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER));
//	return 0;
//}
LRESULT Window::onSize(WPARAM wParam, LPARAM lParam) {
	//SYSPR(SetWindowPos(hWnd_, NULL, 0, 0, width_, height_, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOZORDER));
	//return 0;
	return DefWindowProc(hWnd_, WM_SIZE, wParam, lParam);
}
LRESULT Window::onCommand(WPARAM wParam, LPARAM lParam) {
	return 0;
}
//int Window::onMouseClick(POINT, int) {
//	return 0;
//}
//int Window::onMouseMove(POINT) {
//	return 0;
//}
int Window::onPaint(HDC hdc, PAINTSTRUCT* ps) {
	return 0;
}
int Window::onDestroy() {
	return 0;
}

NS_FW_WIN_END