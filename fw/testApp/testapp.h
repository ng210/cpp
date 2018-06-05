#include "winapp.h"
#include "win/ctrl.h"
#include "utils/buffer.h"

NS_FW_BASE_USE
NS_FW_WIN_USE

class TestApp : public WinApp {
	Ctrl* logControl_;
	Buffer log_;
	int state_;
	static char* eos_;

	void log(const char* text);
protected:
	LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	TestApp(CREATESTRUCT* createStruct, WNDCLASSEX* wndClassEx);
	~TestApp();

	void update();

	LRESULT onCreate();
	//int onCommand(int);
	//int onMouseClick(POINT, int);
	//int onMouseMove(POINT);
	//int onPaint(HDC hdc, PAINTSTRUCT* ps);
	//int onDestroy();

};