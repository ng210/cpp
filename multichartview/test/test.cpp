#include "test.h"
#include "testseries.h"
#include "utils/utils.h"

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
	log_.append("", 1);
	create(createStruct, wndClassEx);
}

TestApp::~TestApp() {
	//DEL_(multiView);
}

LRESULT CALLBACK TestApp::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT ret = 0;
	switch (uMsg) {
		case WM_GETMINMAXINFO:
			MINMAXINFO* mmi;
			mmi = (MINMAXINFO*)lParam;
			mmi->ptMaxSize.x = 1920; mmi->ptMaxSize.y = 1280;
			//mmi->ptMaxPosition.x = 20; mmi->ptMaxPosition.y = 20;
			mmi->ptMaxTrackSize.x = 1920; mmi->ptMaxTrackSize.y = 1280;
			mmi->ptMinTrackSize.x = 320; mmi->ptMinTrackSize.y = 200;
			break;
		case WM_SIZE:
			RECT rect;
			SYSPR(GetClientRect(hWnd_, &rect));
			rect.left += 2; rect.right -= 4;
			rect.top += 2; rect.bottom -= 4;
			SYSFN(ret, SetWindowPos(multiChartView_->hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
			break;

		default:
			ret = Window::wndProc(hWnd, uMsg, wParam, lParam);
	}
	return ret;
}

int TestApp::onCreate() {
	SYSPR(SetWindowText(hWnd_, "MultiChartView test"));
	// prepare data and dataseries
	Array* array = NEW_(Array, sizeof(TESTDATA));
	for (int i = 0; i < SERIESLENGTH; i++) {
		int v = (int)Utils::random(100);
		TESTDATA testData = { v, (float)v/SERIESLENGTH };
		array->add(&testData);
	}
	DataSeries* dataSeries = NEW_(TestSeries, array);
	// create multichartview
	multiChartView_ = NEW_(MultiChartView, this, MYCONTROLID);
	CHARTCHANNELINFO* channels = MALLOC(CHARTCHANNELINFO, 2);
	int ci = 0;
	channels[ci].min = 0.0f; channels[ci].max = 100.0f; channels[ci].color = 0xA04020; ci++;
	channels[ci].min = 0.0f; channels[ci].max = 1.0f; channels[ci].color = 0x40A020;
	multiChartView_->setDataSource(dataSeries, 2, channels);
	//multiChartView_->configuration().stepSize.x = 2;
	//multiChartView_->wndClass();
	RECT rect;
	//multiChartView_ = NEW_(EditCtrl, this, MYCONTROLID);
	SYSPR(GetClientRect(hWnd_, &rect));
	SYSPR(SetWindowPos(multiChartView_->hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));

	//multiView = NEW_(MultiView, hwnd_, MYCONTROLID);
	//multiView->create(hwnd_, MYCONTROLID);
	//multiView->setPosition(0, 0);
	//multiView->setSize(320, 200);
	//SYS(ShowWindow(multiView->hwnd(), SW_SHOWNORMAL));
	//SYS(UpdateWindow(multiView->hwnd()));
	return 0;
}

//int TestApp::onPaint(HDC hdc, PAINTSTRUCT* ps) {
//	return 1;
//}

int TestApp::onDestroy() {
	FREE(multiChartView_->channels());
	return 0;
}