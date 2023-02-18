#include "win/winapp.h"
#include "win/editctrl.h"
#include "utils/buffer.h"
#include "test-ctrl.h"

NS_FW_BASE_USE
NS_FW_WIN_USE

#define EditCtrlId 0x001
#define BtnCtrl1Id 0x002
#define BtnCtrl2Id 0x003

class TestApp : public WinApp {
	Buffer log_;
	int state_;
	static char* eos_;
	TestCtrl testCtrl_;

	void log(const char* text);
protected:
	EditCtrl logCtrl_;
	LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	TestApp();
	~TestApp();

	void create(WndClass wndClass, LONG style = 0, DWORD exStyle = 0);
	void update();

	LRESULT onCreate();
	LRESULT onDestroy();

	LRESULT onLeftUp(POINT& pos, WPARAM state);
};