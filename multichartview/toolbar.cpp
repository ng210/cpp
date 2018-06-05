#include "toolbar.h"
#include "multichartview.h"

NS_FW_WIN_BEGIN

WINDOWCLASS MultiChartViewToolbar::toolbarWndClass_;

MultiChartViewToolbar::MultiChartViewToolbar(Window* parent, int ctrlId) : Ctrl(parent, ctrlId) {
	CREATESTRUCT* createStruct = Ctrl::getCreateStruct("", MULTICHARTVIEW_TOOLBAR_WCLASS);
	WNDCLASSEX wndClassEx = {
		sizeof(WNDCLASSEX),				// UINT        cbSize;
										/* Win 3.x */
		CS_HREDRAW | CS_VREDRAW,		// UINT        style;
		NULL,							// WNDPROC     lpfnWndProc;
		0,								// int         cbClsExtra;
		0,								// int         cbWndExtra;
		parent->hInstance(),			// HINSTANCE   hInstance;
		NULL,							// HICON       hIcon;
		LoadCursor(NULL, IDC_ARROW),	// HCURSOR     hCursor;
		(HBRUSH)COLOR_BACKGROUND,		// HBRUSH      hbrBackground;
		NULL,							// LPCSTR      lpszMenuName;
		MULTICHARTVIEW_TOOLBAR_WCLASS,	// LPCSTR      lpszClassName;
										/* Win 4.0 */
		NULL							// HICON       hIconSm;
	};
	WNDCLASSEX* pWndClassEx = MultiChartViewToolbar::toolbarWndClass_.v != 0 ? NULL : &wndClassEx;
	//createStruct->style |= WS_VSCROLL;
	//createStruct->dwExStyle |= WS_EX_PALETTEWINDOW;
	hWnd_ = create(createStruct, parent, pWndClassEx);
	MultiChartViewToolbar::toolbarWndClass_ = wndClass_;
	FREE(createStruct);
}

MultiChartViewToolbar::~MultiChartViewToolbar() {
	DEL_(channelSelect_);
}

//LRESULT CALLBACK MultiChartViewToolbar::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
//	switch (uMsg) {
//	case WM_COMMAND:
//		
//		break;
//	case WM_ERASEBKGND:
//		return (LRESULT)1;
//	}
//	return Window::wndProc(hWnd, uMsg, wParam, lParam);
//}

LRESULT MultiChartViewToolbar::onCommand(WPARAM wParam, LPARAM lParam) {
	if (HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == ID_MULTICHARTVIEW_TOOLBAR_CHANNELSELECT) {
		((MultiChartView*)parent_)->selectChannel(channelSelect_->getSelection());
	}
	return 1;
}


LRESULT MultiChartViewToolbar::onCreate() {
	channelSelect_ = NEW_(ComboboxCtrl, this, ID_MULTICHARTVIEW_TOOLBAR_CHANNELSELECT);
	SYSPR(SetWindowPos(channelSelect_->hWnd(), NULL, 0, 0, 128, 3*MULTICHARTVIEW_TOOLBARHEIGHT, SWP_SHOWWINDOW));
	return 0;
}

NS_FW_WIN_END