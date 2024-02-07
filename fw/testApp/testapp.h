#include "win/winapp.h"
#include "utils/task.h"
#include "win/staticctrl.h"
#include "win/editctrl.h"
#include "win/comboboxctrl.h"
#include "win/buttonctrl.h"
#include "win/chartctrl.h"
#include "win/logctrl.h"
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
	ComboboxCtrl cbSimple_, cbDropDown_, cbDropDownList_;
	StaticCtrl staticText_, staticIcon_, staticBitmap_;
	ButtonCtrl textButton_, imageButton_;
	ChartCtrl chartCtrl_;

	Task* task1_;

	HBRUSH logBgBrush_;

	void log(const char* text, ...);
protected:
	LogCtrl logCtrl_;
	LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	char* const registerWindowClass();
public:
	TestApp();
	~TestApp();

	void create();
	void update();

	LRESULT onCreated();
	LRESULT onClose();
	//LRESULT onDestroy();

	static MOUSEEVENTPROC onLeftUpProc;
};