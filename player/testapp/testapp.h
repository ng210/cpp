#ifndef TEST_APP_H
#define TEST_APP_H

#include "win/winapp.h"
#include "win/logctrl.h"
#include "player/src/player-device-ext.h"
#include "player/testapp/cons-device-ctrl.h"
#include "test.h"
#include "player/testapp/player-test.h"

NS_FW_BASE_USE
NS_FW_WIN_USE

//class InputCtrlTest : public Test {
//	TestApp* app_;
//	Task* task_;
//
//	Value value_;
//	Input* input_;
//	InputCtrl* inputCtrl_;
//
//public:
//	InputCtrlTest(TestApp* app);
//	~InputCtrlTest();
//
//	void runAll(int& totalPassed, int& totalFailed);
//};

class TestApp : public WinApp {
	Buffer log_;
	PlayerTest* tests_;
	HBRUSH logBgBrush_, testAreaBgBrush_;
	int totalPassed, totalFailed;

protected:
	Player* player_;
	PlayerExt* playerExt_;
	PlayerDevice* playerDevice_;
	PlayerDeviceExt* playerDeviceExt_;
	ConsAdapter* consAdapter_;
	ConsDevice* consDevice_;
	ConsDeviceExt* consDeviceExt_;

	//ButtonCtrl nextButton_, restartButton_;
	protected: StaticCtrl testArea_;
	protected: PROP_R(LogCtrl, logCtrl);
	protected: static IConsole* const getConsole(void*);
	//InputCtrl inputCtrl_;
	//ConsDeviceCtrl* conDeviceCtrl_;
	protected: LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	TestApp();
	~TestApp();
	
	char* const registerWindowClass();

	LRESULT onCreated();
	LRESULT onDestroy();

	void update();

	void log(const char* text, ...);
	StaticCtrl* testArea();

	static MOUSEEVENTPROC nextTest;
	static MOUSEEVENTPROC resetTest;
};
#endif