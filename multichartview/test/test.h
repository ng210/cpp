#include "resource.h"
#include "winapp.h"
#include "utils/buffer.h"
#include "utils/file.h"
#include "multichartview.h"
#include "dataseries.h"

#include "win/editctrl.h"

#define MYCONTROLID 0x1234
#define SERIESLENGTH 128

NS_FW_BASE_USE

class TestApp : public WinApp {
	MultiChartView* multiChartView_;
	Buffer log_;

	DataSeries* dataSeries_;

	static CHART_CUSTOM_DRAW drawBox;

	LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
public:
	TestApp::TestApp(CREATESTRUCT* createStruct, WNDCLASSEX* wndClassEx);
	~TestApp();

	LRESULT onCreate();
	//int onPaint(HDC hdc, PAINTSTRUCT* ps);
	//int onDestroy();
};