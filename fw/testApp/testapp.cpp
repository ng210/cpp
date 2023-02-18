#include "testapp.h"
#include "win/editctrl.h"

NS_FW_BASE_USE
NS_FW_WIN_USE

char* TestApp::eos_ = "\n\0";

TestApp::TestApp() {
	state_ = 0;
}

TestApp::~TestApp() {
	// put cleanup code here
	// log buffer?
}

void TestApp::create(WndClass wndClass, LONG style, DWORD exStyle) {
	Window::create(wndClass, NULL, "TestApp", style, exStyle);
}

LRESULT TestApp::onCreate() {
	RECT rect;
	LRESULT result = 0;
	SYSFN(result, SetWindowPos(hWnd_, NULL, 0, 0, 800, 600, SWP_NOMOVE));
	SYSPR(GetClientRect(hWnd_, &rect));
	HFONT hFont = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);

	// Create edit text control
	logCtrl_.create(this, "", WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL);
	testCtrl_.create(this, "");
	rect.left += 4; rect.right -= 8;
	rect.top += 4; rect.bottom -= 12;
	var height = rect.bottom >> 1;
	SYSFN(result, SetWindowPos(logCtrl_.hWnd(), NULL, rect.left, rect.top, rect.right, height, SWP_SHOWWINDOW));
	rect.top += height + 2;
	SYSPR(SetWindowPos(testCtrl_.hWnd(), NULL, rect.left, rect.top, rect.right, height, SWP_SHOWWINDOW));

	SendMessage(logCtrl_.hWnd(), WM_SETFONT, (WPARAM)hFont, false);

	//char eos = '\0';
	//log_.append(&eos, 1);
	return result;
}

LRESULT TestApp::onDestroy() {
	return 0;
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
		//case WM_SIZE:
		//	break;
		default:
			ret = Window::wndProc(hWnd, uMsg, wParam, lParam);
	}
	return ret;
}

LRESULT TestApp::onLeftUp(POINT& pos, WPARAM state) {
	if ((state_ & 0x01) != 0) {
		state_++;
	}
	return 0;
}

void TestApp::log(const char* text) {
	// log text
	log_.write((void*)text, NS_FW_BASE::strlen(text), 0, log_.length()-1);
	char* eol = "\r\n---\r\n\0";
	log_.append(eol, 8);
	char* buffer = log_.getBufferAsType<char>();
	SetWindowText(logCtrl_.hWnd(), buffer);
	testCtrl_.setText(buffer);
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


WinApp* createApplication(HINSTANCE hInstance, Map* args) {
	// Load menu
	HMENU hMenu = NULL;
	var wndClass = Window::registerClass((char*)"TestApp", hInstance);
	var app = NEW_(TestApp);
	app->create(wndClass);
	return app;
}
