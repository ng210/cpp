#include "testapp.h"
#include "win/editctrl.h"

NS_FW_BASE_USE
NS_FW_WIN_USE

char* TestApp::eos_ = "\n\0";

WinApp* createApplication(HINSTANCE hInstance, Map* args) {
	// Load menu
	HMENU hMenu = NULL;

	CREATESTRUCT createStruct = {
		NULL,							// LPVOID      lpCreateParams;
		hInstance,						// HINSTANCE   hInstance;
		hMenu,							// HMENU       hMenu;
		NULL,							// HWND        hwndParent;
		480,							// int         cy;
		640,							// int         cx;
		CW_USEDEFAULT,					// int         y;
		CW_USEDEFAULT,					// int         x;
		WS_OVERLAPPEDWINDOW,			// LONG        style;
		"Test App",						// LPCSTR      lpszName;
		0,								// LPCSTR      lpszClass;
		0								// DWORD       dwExStyle;
	};
	WNDCLASSEX wndClassEx = {
		sizeof(WNDCLASSEX),				// UINT        cbSize;
										/* Win 3.x */
		CS_HREDRAW | CS_VREDRAW,		// UINT        style;
		NULL,							// WNDPROC     lpfnWndProc;
		0,								// int         cbClsExtra;
		0,								// int         cbWndExtra;
		hInstance,						// HINSTANCE   hInstance;
		LoadIcon(NULL, IDI_APPLICATION),// HICON       hIcon;
		LoadCursor(NULL, IDC_ARROW),	// HCURSOR     hCursor;
		(HBRUSH)COLOR_BACKGROUND,		// HBRUSH      hbrBackground;
		NULL,							// LPCSTR      lpszMenuName;
		"TestWClass",					// LPCSTR      lpszClassName;
										/* Win 4.0 */
		NULL							// HICON       hIconSm;
	};
	return NEW_(TestApp, &createStruct, &wndClassEx);
}

TestApp::TestApp(CREATESTRUCT* createStruct, WNDCLASSEX* wndClassEx) {
	// put initialization code here
	logControl_ = NULL;
	char eos = '\0';
	state_ = 0;
	log_.append(&eos, 1);
	create(createStruct, wndClassEx);
}
TestApp::~TestApp() {
	// put cleanup code here
	DEL_(logControl_);
	// log buffer?
}

LRESULT CALLBACK TestApp::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT ret = 0;
	switch (uMsg) {
		//case WM_SIZING:
		//	sizing((int)wParam, (LPRECT)lParam, NULL, 640, 400);
		//	ret = TRUE;
		//	break;
		case WM_GETMINMAXINFO:
			MINMAXINFO* mmi;
			mmi = (MINMAXINFO*)lParam;
			mmi->ptMaxSize.x = 800; mmi->ptMaxSize.y = 600;
			//mmi->ptMaxPosition.x = 20; mmi->ptMaxPosition.y = 20;
			mmi->ptMaxTrackSize.x = 800; mmi->ptMaxTrackSize.y = 600;
			mmi->ptMinTrackSize.x = 640; mmi->ptMinTrackSize.y = 400;
			break;
		case WM_SIZE:
			RECT rect;
			//SYSPR(GetClientRect(hWnd_, &rect));
			SYSPR(GetClientRect(hWnd_, &rect));
			rect.left += 4; rect.right -= 8;
			rect.top += 4; rect.bottom -= 8;
			SYSFN(ret, SetWindowPos(logControl_->hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
			break;
		case WM_LBUTTONUP:
			if ((state_ & 0x01) != 0) {
				state_++;
			}
			break;
		default:
			ret = Window::wndProc(hWnd, uMsg, wParam, lParam);
	}
	return ret;
}

void TestApp::log(const char* text) {
	// log text
	log_.write((void*)text, NS_FW_BASE::strlen(text), 0, log_.length()-1);
	char* eol = "\r\n---\r\n\0";
	log_.append(eol, 8);
	char* buffer = log_.getBufferAsType<char>();
	SetWindowText(logControl_->hWnd(), buffer);
	FREE(buffer);
	//ScrollWindow(logControl_->hWnd(), 0, 0, NULL, &rect);
}

void TestApp::update() {
	switch (state_) {
		case 0:
			log("State 0: Click to advance state!");
			state_++;
			break;
		case 2:
			log("State 1: Hello world!");
			state_++;
			break;
		case 4:
			log("State 2: TestApp works!");
			state_++;
			break;
		case 6:
			log("State 3: Final state reached, restarting!");
			state_ = -1;
			break;
		default:
			break;
	}
	Sleep(1);
}

LRESULT TestApp::onCreate() {
	// Create static text control
	RECT rect;
	SYSPR(GetClientRect(hWnd_, &rect));
	logControl_ = NEW_(EditCtrl, this, 0x001, WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL, NULL);
	//LONG_PTR SYSFN(style, GetWindowLongPtr(logControl_->hWnd(), GWL_STYLE));
	//style = WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL;
	//SYSPR(SetWindowLongPtr(logControl_->hWnd(), GWL_STYLE, style));
	SYSPR(SetWindowPos(logControl_->hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
	HFONT hFont = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);
	SendMessage(logControl_->hWnd(), WM_SETFONT, (WPARAM)hFont, false);
	return 0;
}

//int TestApp::onCommand(int wmId) {
//	return 0;
//}
//int TestApp::onMouseClick(POINT, int) {
//	return 0;
//}
//int TestApp::onMouseMove(POINT) {
//	return 0;
//}
//int TestApp::onPaint(HDC hdc, PAINTSTRUCT* ps) {
//	return 0;
//}
//int TestApp::onDestroy() {
//	return 0;
//}