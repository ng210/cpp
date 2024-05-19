#include <math.h>
#include "synth/testapp/testapp.h"
#include "utils/task.h"

NS_FW_BASE_USE
NS_FW_WIN_USE

char* windowClassName = "SynthTestApp";

#define LOGBGCOLOR 0x00402010
#define LOGTEXTCOLOR 0x0080C0E0
#define TESTAREABGCOLOR 0x806050

TestApp::TestApp() {
	player_ = NULL;
	playerExt_ = NULL;
	playerDevice_ = NULL;
	playerDeviceExt_ = NULL;
	//inputCtrl_ = NULL;
	//conDeviceCtrl_ = NULL;
	tests_ = NEW_(SynthTest, this);
	logBgBrush_ = NULL;
	testAreaBgBrush_ = NULL;
	totalFailed = 0;
	totalPassed = 0;
}

TestApp::~TestApp() {
	DEL_(tests_);
	SYSPR(DeleteObject(logBgBrush_));
}

char* const TestApp::registerWindowClass() {
	Window::registerClass(windowClassName, hInstance_);
	return windowClassName;
}

IConsole* const TestApp::getConsole(void* obj) {
	return (LogCtrl*)obj;
}

LRESULT TestApp::onCreated() {
	LRESULT result = 0;
	int width = 1340, height = 960;
	int margin = 4;
	HFONT SYSFN(hFont, (HFONT)GetStockObject(SYSTEM_FIXED_FONT));
	SYSFN(result, SetWindowPos(hWnd_, NULL, 0, 0, width, height, SWP_NOMOVE));
	RECT rect;
	SYSPR(GetClientRect(hWnd_, &rect));
	rect.left += margin, rect.top += margin;
	rect.right -= 2*margin, rect.bottom -= 2*margin;
	int px = rect.left, py = rect.top;

	#pragma region Init test area
	//px = rect.left, py += nextButton_.rect().bottom + margin;
	testArea_.create(this, "");
	testArea_.set(px, py, rect.right, 3*rect.bottom>>2);
	py += testArea_.rect().bottom + margin;
	SYSFN(testAreaBgBrush_, CreateSolidBrush(TESTAREABGCOLOR));
	#pragma endregion

	#pragma region Init log control	
	logCtrl_.create(this, "");
	logCtrl_.set(px, py, rect.right, +margin + rect.bottom - py);
	SYSPR(SendMessage(logCtrl_.hWnd(), WM_SETFONT, (WPARAM)hFont, false));
	SYSFN(logBgBrush_, CreateSolidBrush(LOGBGCOLOR));
	#pragma endregion

	tests_->cons = &logCtrl_;
	tests_->runAll(totalPassed, totalFailed);

	return result;
}

LRESULT TestApp::onDestroy() {
	log_.clear();
	return 0;
}

LRESULT CALLBACK TestApp::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT ret = 0;
	switch (uMsg) {
		case WM_CTLCOLORSTATIC:
			HWND ctrl; ctrl = (HWND)lParam;
			HDC hDC; hDC = (HDC)wParam;
			if (ctrl == logCtrl_.hWnd()) {
				// set color for log
				SetTextColor(hDC, LOGTEXTCOLOR);
				SetBkColor(hDC, LOGBGCOLOR);
				return (INT_PTR)logBgBrush_;
			}
			else if (ctrl == testArea_.hWnd()) {
				SetTextColor(hDC, TESTAREABGCOLOR);
				return (INT_PTR)testAreaBgBrush_;
			}
		default:
			ret = Window::wndProc(hWnd, uMsg, wParam, lParam);
	}
	return ret;
}

void TestApp::log(const char* text, ...) {
	va_list args;
	va_start(args, text);
	logCtrl_.vprintf(text, args);
	va_end(args);
}

void TestApp::update() {
}

StaticCtrl* TestApp::testArea() {
	return &testArea_;
}

WinApp* createApplication(HINSTANCE hInstance, Map* args) {
	// Load menu
	HMENU hMenu = NULL;
	var wndClass = Window::registerClass((char*)"TestApp", hInstance);
	var app = NEW_(TestApp);
	app->create(NULL, windowClassName);
	return app;
}
