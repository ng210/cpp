#include "synapp.h"
#include <stdlib.h>

#define MAX_LOADSTRING 100

// Global Variables:
HWND g_hWnd;
HINSTANCE g_hInst;
//Logic g_Logic;

TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass();
BOOL InitInstance(int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

/*****************************************************************************/
ChartCtrl* g_chart;
SArray* g_dataSerie;

int processCommand(HWND hWnd, int wmId, int wmEvent) {
	int ret = 1;
	switch (wmId) {
		case IDM_ABOUT:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			ret = 0;
	}
	return ret;
}

//#ifndef __CRT
//int APIENTRY __tWinMain(HINSTANCE hInstance, Collection &args, int nCmdShow);
//int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
//{
//	Collection *args = strsplit(lpCmdLine, " \t", "\"", NULL);
//	int ret = __tWinMain(hInstance, *args, nCmdShow);
//	DEL(args);
//#ifdef _DEBUG
//	_MEM_DBG_check();
//#endif
//	return ret;
//}
//#endif
int APIENTRY _WinMain(HINSTANCE hInstance, IndexedCollection &args, int nCmdShow) {
	//UNREFERENCED_PARAMETER(hPrevInstance);
	//UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	g_hInst = hInstance;
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SYNAPP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass();

	// Perform application initialization:
	if (!InitInstance (nCmdShow)) {
		return FALSE;
	}

	hAccelTable = LoadAccelerators(g_hInst, MAKEINTRESOURCE(IDC_SYNAPP));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

void createTestData() {
	// create test data
	NEW(g_dataSerie, SArray, 16);
	for (int s=0; s<4; s++) {
		// add series
		SArray* NEW(pSeries, SArray, sizeof(POINT), 100);
		// add points
		int x = rand() % 5, y = rand() % 100, cy, dy;
		while (pSeries->Count() < 8) {
			POINT* p = (POINT*)pSeries->add().p;
			cy = y; y += (dy = (rand()%10 - 20));
			if (y < 0 || y > 100) y = cy - dy;
			p->x = x; p->y = y;
			_VS_debug("Point (%d, %d)\n", x, y);
			x += rand() % 5 + 8;
		}
		COLLECTIONITEM item = g_dataSerie->add();
		*item.ref = pSeries;
	}
}

ATOM MyRegisterClass() {
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= g_hInst;
	wcex.hIcon			= LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_SYNAPP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SYNAPP);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(int nCmdShow) {
   g_hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 640, 480, NULL, NULL, g_hInst, NULL);
   if (g_hWnd) {
	   ShowWindow(g_hWnd, nCmdShow);
	   UpdateWindow(g_hWnd);
   }
   return (BOOL)g_hWnd;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (uMsg) {
		case WM_CREATE:
			//if (g_Logic.init(g_hInst, hWnd)) {
			//	MessageBox(hWnd, "Error", "SynApp", MB_OK);
			//}
			RECT rect;
			SYS(GetClientRect(hWnd, &rect));
			NEW(g_chart, ChartCtrl, hWnd, 1234, NULL);
			//g_panel->init(hWnd, 1234, NULL);
			// position panel at the top-left corner
			g_chart->setPosition(0, 0);
			// set panel's size to cover the whole client area
			g_chart->setSize(rect.right, rect.bottom);
			createTestData();
			g_chart->setData(g_dataSerie, 256, 128);
			break;
		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			if (!processCommand(hWnd, wmId, wmEvent)) {
				goto _default;
			}
			break;
		case WM_SIZE:
		//case WM_SIZING:
			SYS(SendMessage(g_chart->Wnd(), uMsg, wParam, lParam));
			goto _default;
			break;
		//case WM_PAINT:
		//	hdc = BeginPaint(hWnd, &ps);
		//	// TODO: Add any drawing code here...
		//	EndPaint(hWnd, &ps);
		//	break;
		case WM_CLOSE:
			//g_Logic.destroy(g_hInst, g_hWnd);
			DestroyWindow(hWnd);
			DEL(g_chart);
			DEL(g_dataSerie);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
_default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
		case WM_INITDIALOG:
			return (INT_PTR)TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
	}
	return (INT_PTR)FALSE;
}
