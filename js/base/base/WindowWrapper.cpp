#include "base/WindowWrapper.h"
//#include "base/debug.h"

NS_FW_BASE_BEGIN

WindowWrapper::WindowWrapper() {
}
WindowWrapper::~WindowWrapper() {

}

void WindowWrapper::createWindow(ATOM wndClass, HWND parent, HMENU hmenu, int style) {
	hwnd_ = SYS(CreateWindowEx(
		0,
		MAKEINTATOM(wndClass), NULL,
		style,
		CW_USEDEFAULT, 0,
		CW_USEDEFAULT, 0,
		parent,
		hmenu,
		instance_,
		this)
	);
}

ATOM WindowWrapper::registerClassByName(LPCTSTR className) {
	// register window class
	WNDCLASSEX wndClassEx = {
		sizeof(WNDCLASSEX),
		CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
		&WindowWrapper::wndProcWrapper,
		0,
		sizeof(WindowWrapper*),
		instance_,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(BLACK_BRUSH + 1),
		NULL,
		className,
		NULL
	};
	return SYS(RegisterClassEx(&wndClassEx));
}

LRESULT CALLBACK WindowWrapper::wndProcWrapper(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT ret = 0;
	if (message == WM_CREATE) {
		WindowWrapper* windowWrapper = *(WindowWrapper**)lParam;
		windowWrapper->hwnd_ = hWnd;
		SYS(SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)windowWrapper));
		ret = (windowWrapper->wndProc)(hWnd, message, wParam, lParam);
	}
	else {
		WindowWrapper* windowWrapper = NULL;
		windowWrapper = (WindowWrapper*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		DWORD dwError = GetLastError();
		if (dwError != 0) {
			DEBUG(TEXT("WindowWrapper::wndProcWrapper hwnd: %X\n"), hWnd);
			DEBUG(TEXT("WindowWrapper::wndProcWrapper ptr: %lX\n"), windowWrapper);
			const TCHAR* szError = tracer_->getLastErrorW();
			if (szError != NULL && wcslen(szError) > 0)
				DEBUG(szError);
				tracer_->debugW(szError, __TFILE__, __LINE__);
			LocalFree((LPVOID)szError);
		}
		// WindowWrapper* windowWrapper = (WindowWrapper*)SYS(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		//Debug::print()("windowWrapper: %x\n", windowWrapper);
		if (windowWrapper != NULL) {
			ret = (windowWrapper->wndProc)(hWnd, message, wParam, lParam);
		}
		else {
			ret = SYS(DefWindowProc(hWnd, message, wParam, lParam));
		}
	}
	return ret;
}
LRESULT CALLBACK WindowWrapper::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT ret = 0;
	switch (message) {
	case WM_CREATE:
		onCreate();
		break;
	case WM_COMMAND:
		onCommand(LOWORD(wParam));
		break;
	case WM_PAINT:
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = SYS(BeginPaint(hWnd, &ps));
		ret = onPaint(hdc, &ps);
		SYS(EndPaint(hWnd, &ps));
		break;
	case WM_DESTROY:
		onDestroy();
		PostQuitMessage(0);
		break;
	}
	if (ret == 0) {
		ret = SYS(DefWindowProc(hWnd, message, wParam, lParam));
	}
	return ret;
}

LRESULT WindowWrapper::onCreate() {
	return 0;
}
LRESULT WindowWrapper::onCommand(int wmId) {
	return 0;
}
LRESULT WindowWrapper::onPaint(HDC hdc, PAINTSTRUCT* ps) {
	return 0;
}
LRESULT WindowWrapper::onDestroy() {
	return 0;
}

NS_FW_BASE_END