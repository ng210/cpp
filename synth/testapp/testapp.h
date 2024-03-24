#ifndef TEST_APP_H
#define TEST_APP_H

#include "win/winapp.h"
#include "win/logctrl.h"
#include "player/src/player-device-ext.h"
#include "synth/testapp/synth-test.h"

NS_FW_BASE_USE
NS_FW_WIN_USE

class TestApp : public WinApp {
	Buffer log_;
	SynthTest* tests_;
	HBRUSH logBgBrush_, testAreaBgBrush_;
	int totalPassed, totalFailed;

protected:
	Player* player_;
	PlayerExt* playerExt_;
	PlayerDevice* playerDevice_;
	PlayerDeviceExt* playerDeviceExt_;

	protected: StaticCtrl testArea_;
	protected: PROP_R(LogCtrl, logCtrl);
	protected: static IConsole* const getConsole(void*);
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