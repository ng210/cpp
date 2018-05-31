#include "multichartview.h"
#include "windowsx.h"

NS_FW_WIN_BEGIN

#define CHART_TO_X(i) ((i)*(2.0f + configuration_.stepSize.x / scaleX_))
#define CHART_FROM_X(cx) ((cx) / (2 + configuration_.stepSize.x / scaleX_))

ATOM MultiChartView::class_ = 0;
WNDCLASSEX MultiChartView::wndClassEx_ = {
	sizeof(WNDCLASSEX),				// UINT        cbSize;
									/* Win 3.x */
	CS_HREDRAW | CS_VREDRAW,		// UINT        style;
	NULL,							// WNDPROC     lpfnWndProc;
	0,								// int         cbClsExtra;
	0,								// int         cbWndExtra;
	NULL,							// HINSTANCE   hInstance;
	LoadIcon(NULL, IDI_APPLICATION),// HICON       hIcon;
	LoadCursor(NULL, IDC_ARROW),	// HCURSOR     hCursor;
	NULL,							// HBRUSH      hbrBackground;
	NULL,							// LPCSTR      lpszMenuName;
	"MultiChartViewWClass",			// LPCSTR      lpszClassName;
									/* Win 4.0 */
	NULL							// HICON       hIconSm;
};

MultiChartView::MultiChartView(Window* parent, size_t ctrlId) : Ctrl(parent, ctrlId), selection_(sizeof(int)) {
	dataSeries_ = NULL;
	offsetX_ = 0;
	offsetY_ = 0;

	scaleX_ = 1.0f;
	scaleY_ = 1.0f;

	// default configuration
	configuration_ = {
		0x401000,		// background color
		{ 8, 4 },		// stepsize
		{ 8, 8 }		// grid
	};

	// register class
	WNDCLASSEX* wndClassEx = MultiChartView::class_ == 0 ? &MultiChartView::wndClassEx_ : NULL;
	int style = WS_CHILD | WS_VISIBLE | WS_HSCROLL;
	CREATESTRUCT createStruct = {
		NULL,							// LPVOID      lpCreateParams;
		parent->hInstance(),			// HINSTANCE   hInstance;
		(HMENU)ctrlId,					// HMENU       hMenu;
		parent->hWnd(),					// HWND        hwndParent;
		CW_USEDEFAULT,					// int         cy;
		CW_USEDEFAULT,					// int         cx;
		CW_USEDEFAULT,					// int         y;
		CW_USEDEFAULT,					// int         x;
		style,							// LONG        style;
		"MultiChartView",				// LPCSTR      lpszName;
		"MultiChartViewClass",			// LPCSTR      lpszClass;
		0								// DWORD       dwExStyle;
	};
	// create control
	hWnd_ = create(&createStruct, parent, wndClassEx);

	isSelecting_ = 0;
	hSelectionBitmap_ = NULL;
}

MultiChartView::~MultiChartView() {
	DeleteObject(hSelectionBitmap_);
	DeleteDC(selectHdc_);
}

void MultiChartView::setDataSource(DataSeries* source, int channelCount, CHARTCHANNELINFO* channels) {
	// initialization
	dataSeries_ = source;
	channelCount_ = channelCount;
	channels_ = channels;
	//minY_ = 1000000;
	//maxY_ = -1000000;
	//for (int ch = 0; ch < channelCount; ch++) {
	//	minY_ = min(minY_, channels[ch].min);
	//	maxY_ = max(maxY_, channels[ch].max);
	//}
}

void MultiChartView::select() {
	for (int x = selectRect_.left; x < selectRect_.right; x++) {
		int ix = (int)CHART_FROM_X(x + offsetX_);
		int at = 0;
		if (selection_.binSearch(&ix, at, Collection::compareInt) == NULL) {
			selection_.insertAt(at, &ix);
		}
	}
	InvalidateRect(hWnd_, NULL, false);
}

void MultiChartView::draw(LPARAM lParam) {
	int ch = 0;
	// get x,y from mouse click
	int cx = GET_X_LPARAM(lParam) + offsetX_;
	int cy = GET_Y_LPARAM(lParam);
	if (cy < MULTICHARTVIEW_TOOLBARHEIGHT) {
		cy = MULTICHARTVIEW_TOOLBARHEIGHT;
	}
	int ix = (int)CHART_FROM_X(cx);
	RECT rect;
	SYSPR(GetClientRect(hWnd_, &rect));
	double y = (double)(rect.bottom - cy) / (rect.bottom - rect.top - MULTICHARTVIEW_TOOLBARHEIGHT);
	int at = 0;
	if (true || selection_.binSearch(&ix, at, Collection::compareInt) != NULL) {
		dataSeries_->set(ix, ch, y);
	}
	InvalidateRect(hWnd_, NULL, false);
}

//void MultiChartView::setGrid(int x, int y) {
//	gridX_ = x > 4 ? x < 16 ? x : 16 : 4;
//	gridY_ = y > 4 ? y < 16 ? y : 16 : 4;
//	InvalidateRect(hWnd_, NULL, true);
//}
int maxSize = 0;
int xMaxScroll = 0;
int xCurrentScroll = 0;
LRESULT CALLBACK MultiChartView::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_SIZE:
			onSize(wParam, lParam);
			DefWindowProc(hWnd, uMsg, wParam, lParam);
			break;
		case WM_HSCROLL:
			onScrollX(wParam, lParam);
			break;
		case WM_LBUTTONDOWN:
			onLeftDown(wParam, lParam);
			break;
		case WM_LBUTTONUP:
			onLeftUp(wParam, lParam);
			break;
		case WM_RBUTTONDOWN:
			onRightDown(wParam, lParam);
			break;
		case WM_RBUTTONUP:
			onRightUp(wParam, lParam);
			break;
		case WM_MOUSEMOVE:
			onMouseMove(wParam, lParam);
			break;
		case WM_ERASEBKGND:
			return (LRESULT)1;
	}
	return Window::wndProc(hWnd, uMsg, wParam, lParam);
}

void MultiChartView::onSize(WPARAM wParam, LPARAM lParam) {
	if (dataSeries_ != NULL) {
		//SYSPR(GetClientRect(hWnd_, &rect));
		//UINT32 width = rect.right - rect.left;
		UINT32 width = LOWORD(lParam);
		int maxWidth = (int)CHART_TO_X(dataSeries_->getLength());
		maxX_ = maxWidth - width;
		offsetX_ = min(offsetX_, maxX_);
		SCROLLINFO si = {
			sizeof(SCROLLINFO),				// cbSize
			SIF_RANGE | SIF_PAGE | SIF_POS,	// fMask
			0,								// nMin
			maxWidth,						// nMax
			width,							// nPage
			offsetX_						// nPos
		};
		SetScrollInfo(hWnd_, SB_HORZ, &si, false);
	}
}

void MultiChartView::onScrollX(WPARAM wParam, LPARAM lParam) {
	int xDelta;
	int xNewPos;

	int pageSize = (int)CHART_TO_X(4);
	int lineSize = (int)CHART_TO_X(1);

	switch (LOWORD(wParam)) {
	case SB_PAGEUP:
		xNewPos = offsetX_ - pageSize;
		break;
	case SB_PAGEDOWN:
		xNewPos = offsetX_ + pageSize;
		break;
	case SB_LINEUP:
		xNewPos = offsetX_ - lineSize;
		break;
	case SB_LINEDOWN:
		xNewPos = offsetX_ + lineSize;
		break;
	case SB_THUMBPOSITION:
		xNewPos = HIWORD(wParam);
		break;
	default:
		xNewPos = offsetX_;
	}

	xNewPos = max(0, xNewPos);
	xNewPos = min(maxX_, xNewPos);

	if (xNewPos != offsetX_) {
		xDelta = xNewPos - offsetX_;
		offsetX_ = xNewPos;
		//ScrollWindowEx(hWnd_, -xDelta, 0, (CONST RECT *) NULL, (CONST RECT *) NULL, (HRGN)NULL, (PRECT)NULL, SW_INVALIDATE);
		InvalidateRect(hWnd_, NULL, false);
		UpdateWindow(hWnd_);

		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		si.nPos = offsetX_;
		SetScrollInfo(hWnd_, SB_HORZ, &si, TRUE);
	}
}

void MultiChartView::onLeftDown(WPARAM wParam, LPARAM lParam) {
	// get x from mouse click
	isSelecting_ = true;
	SYSPR(GetClientRect(hWnd_, &selectRect_));
	selectRect_.bottom -= MULTICHARTVIEW_TOOLBARHEIGHT;
	dragStartX_ = GET_X_LPARAM(lParam);
	if ((wParam & MK_CONTROL) == 0) {
		selection_.clear();
	}
	//selectRect_.bottom = selectRect_.top = GET_Y_LPARAM(lParam) - MULTICHARTVIEW_TOOLBARHEIGHT;
}

void MultiChartView::onLeftUp(WPARAM wParam, LPARAM lParam) {
	isSelecting_ = false;
	this->select();
}

void MultiChartView::onRightDown(WPARAM wParam, LPARAM lParam) {
	//isDragging_ = true;
	draw(lParam);
}

void MultiChartView::onRightUp(WPARAM wParam, LPARAM lParam) {
	//isDragging_ = false;
}

void MultiChartView::onMouseMove(WPARAM wParam, LPARAM lParam) {
	//if (isDragging_) {
		if (wParam & MK_LBUTTON) {
			long x = GET_X_LPARAM(lParam);
			selectRect_.left = min(dragStartX_, x);
			selectRect_.right = max(dragStartX_, x);
			InvalidateRect(hWnd_, NULL, false);
		}
		if (wParam & MK_RBUTTON) {
			draw(lParam);
		}
	//}
}

DWORD addColors(DWORD color1, DWORD color2) {
	DWORD col = 0;
	for (int c = 0; c < 3; c++) {
		int cc = ((UINT8*)&color1)[c] + ((UINT8*)&color2)[c];
		((UINT8*)&col)[c] = cc > 0xFF ? 0xFF : cc;
	}
	return col;
}

int MultiChartView::onPaint(HDC hdc, PAINTSTRUCT* ps) {
	RECT clientRect, toolbarRect, frameRect;
	SYSPR(GetClientRect(hWnd_, &clientRect));

	// paint toolbar
	CopyRect(&toolbarRect, &clientRect);
	toolbarRect.bottom = MULTICHARTVIEW_TOOLBARHEIGHT;
	HBRUSH toolbarColor = GetSysColorBrush(COLOR_3DFACE);
	SYSPR(FillRect(hdc, &toolbarRect, toolbarColor));
	clientRect.bottom -= MULTICHARTVIEW_TOOLBARHEIGHT;

	HDC SYSFN(hdcMem, CreateCompatibleDC(hdc));
	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;
	HBITMAP SYSFN(hBitMap, CreateCompatibleBitmap(hdc, width, height));
	HBITMAP SYSFN(hBitMapOld, (HBITMAP)SelectObject(hdcMem, hBitMap));

	if (hSelectionBitmap_ == NULL) {
		SYSFN(selectHdc_, CreateCompatibleDC(hdc));
		SYSFN(hSelectionBitmap_, CreateCompatibleBitmap(hdc, 1, 1));
		HBRUSH SYSFN(selectColor, CreateSolidBrush(0xFFFFFF));
		SYSPR(SelectObject(selectHdc_, hSelectionBitmap_));
		RECT rect = { 0,0,1,1 };
		FillRect(selectHdc_, &rect, selectColor);
		DeleteBrush(selectColor);
	}

	// paint background
	HBRUSH SYSFN(backgroundColor, CreateSolidBrush(configuration_.backgroundColor));
	SYSPR(FillRect(hdcMem, &clientRect, backgroundColor));
	DeleteBrush(backgroundColor);

	CopyRect(&frameRect, &clientRect);

	BLENDFUNCTION ftn = {
		AC_SRC_OVER,
		0,
		20,
		0
	};

	if (dataSeries_ != NULL) {
		DWORD gridColor = addColors(configuration_.backgroundColor, 0x101010);
		HBRUSH SYSFN(gridColorBg, CreateSolidBrush(gridColor));
		// get x range
		int dataStart = (int)CHART_FROM_X(offsetX_);
		int dataRange = (int)CHART_FROM_X(clientRect.right - clientRect.left) + dataStart;
		// paint horizontal grid lines
		HPEN SYSFN(gridPen, CreatePen(PS_GEOMETRIC, 0, gridColor));
		float deltaY = (float)height / configuration_.grid.y;
		SYSPR(SelectObject(hdcMem, gridPen));
		for (float y = (float)clientRect.bottom; y > (float)clientRect.top; y -= deltaY) {
			SYSPR(MoveToEx(hdcMem, clientRect.left, (int)y, NULL));
			SYSPR(LineTo(hdcMem, clientRect.right, (int)y));
		}
		// vertical gridlines
		float deltaX = CHART_TO_X(configuration_.grid.x);
		float x = 0;	// (float)(-offsetX_ % (int)(configuration_.grid.x * configuration_.stepSize.x * scaleX_));
		int gridX = dataStart % configuration_.grid.x;
		for (int i = dataStart; i < dataRange; i++) {
			if (++gridX == configuration_.grid.x) {
				frameRect.top = clientRect.top;
				frameRect.bottom = clientRect.bottom;
				frameRect.left = (long)x;
				frameRect.right = (long)(x + configuration_.stepSize.x * scaleX_);
				SYSPR(FillRect(hdcMem, &frameRect, gridColorBg));
				//x += deltaX;
				gridX = 0;
			}
			x += CHART_TO_X(1);
		}
		SYSPR(DeletePen(gridPen));
		for (int ch = 0; ch < channelCount_; ch++) {
			x = 0;
			CHARTCHANNELINFO* ci = &channels_[ch];
			HBRUSH SYSFN(color, CreateSolidBrush(ci->color));
			DWORD gridColor = addColors(ci->color, 0x404040);
			HBRUSH SYSFN(gridBrush, CreateSolidBrush(gridColor));
			SYSFN(gridPen, CreatePen(PS_GEOMETRIC, 0, gridColor));
			SYSPR(SelectObject(hdcMem, gridPen));
			// paint bars
			gridX = dataStart % configuration_.grid.x;
			int px = -1, py = 0;
			for (int i = dataStart; i < dataRange; i++) {
				double value;
				if (dataSeries_->get(i, ch, value)) {
					int y = (int)(value * height);
					frameRect.left = (long)x;
					frameRect.right = (long)(x + configuration_.stepSize.x * scaleX_);
					HBRUSH brush = color;
					if (++gridX == configuration_.grid.x) {
						brush = gridBrush;
						gridX = 0;
					}
					int cy = clientRect.bottom - y;
					frameRect.top = cy - 1;
					frameRect.bottom = cy + 1;
					if (px != -1) {
						SYSPR(MoveToEx(hdcMem, px, py, NULL));
						SYSPR(LineTo(hdcMem, frameRect.left, cy));
					}
					px = frameRect.right; py = cy;
					SYSPR(FillRect(hdcMem, &frameRect, brush));
					// overpaint selection
					int ix = 0;
					if (selection_.binSearch(&i, ix, Collection::compareInt)) {
						SYSPR(AlphaBlend(
							hdcMem, frameRect.left-1, clientRect.top, frameRect.right - frameRect.left+2, height,
							selectHdc_, 0, 0, 1, 1, ftn));
					}
				}
				x += CHART_TO_X(1);
			}
			SYSPR(DeletePen(gridPen));
			SYSPR(DeleteBrush(gridBrush));
			SYSPR(DeleteBrush(color));
		}
		SYSPR(DeleteBrush(gridColorBg));
	}

	// paint selection rectangle
	if (isSelecting_) {
		SYSPR(AlphaBlend(
			hdcMem, selectRect_.left, selectRect_.top, selectRect_.right - selectRect_.left, height,
			selectHdc_, 0, 0, 1, 1, ftn));
	}

	BitBlt(hdc, clientRect.left, MULTICHARTVIEW_TOOLBARHEIGHT, width, height, hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hBitMapOld);
	DeleteObject(hBitMap);
	DeleteDC(hdcMem);

	return 0;
}
//int MultiChartView::onDestroy() {
//	return Window::onDestroy();
//}

NS_FW_WIN_END