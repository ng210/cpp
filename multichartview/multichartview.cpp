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

static DWORD addColors(DWORD color1, DWORD color2) {
	DWORD col = 0;
	for (int c = 0; c < 3; c++) {
		int cc = ((UINT8*)&color1)[c] + ((UINT8*)&color2)[c];
		((UINT8*)&col)[c] = cc > 0xFF ? 0xFF : cc;
	}
	return col;
}
static DWORD mixColors(DWORD color1, DWORD color2, float mix) {
	DWORD col = 0;
	for (int c = 0; c < 3; c++) {
		int cc = (int)(mix*((UINT8*)&color1)[c] + (1.0f - mix) * ((UINT8*)&color2)[c]);
		((UINT8*)&col)[c] = cc > 0xFF ? 0xFF : cc;
	}
	return col;
}

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
		{ 8, 8 },		// grid
		false			// canDrawWithoutSelection
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
	selectedChannel_ = 0;
	lastDrawX_ = 0;

	hdc_ = NULL;
	hBitmap_ = NULL;

	update();
}

MultiChartView::~MultiChartView() {
	DeleteObject(hSelectionBitmap_);
	DeleteDC(selectHdc_);

	//for (int i = 0; i < channelCount_; i++) {
	//	CHARTCHANNELINFO& channel = channels_[i];
	//	DeleteObject(channel.brush_);
	//	DeleteObject(channel.gridBrush_);
	//	DeleteObject(channel.pen_);
	//}

	if (hdc_ != NULL) {
		HBITMAP bitmap = (HBITMAP)SelectObject(hdc_, hBitmap_);
		DeleteObject(bitmap);
		DeleteDC(hdc_);
	}
	DeleteObject(hBackgroundBrush_);
	DeleteObject(hGridBrush_);
	DEL_(toolbar_);
}

void MultiChartView::setDataSource(DataSeries* source, int channelCount, CHARTCHANNELINFO* channels) {
	// initialization
	dataSeries_ = source;
	channelCount_ = channelCount;
	channels_ = channels;
	for (int i = 0; i < channelCount; i++) {
		CHARTCHANNELINFO& ci = channels_[i];
		toolbar_->channelSelect()->addItem((LPTSTR)ci.label);
		//SYSFN(ci.activeBrush_, CreateSolidBrush(ci.color));
		//DWORD color = mixColors(ci.color, configuration_.backgroundColor, 0.3);
		//SYSFN(ci.inactiveBrush_, CreateSolidBrush(color));
		//SYSFN(ci.gridBrush_, CreateSolidBrush(addColors(color, 0x404040)));
		//SYSFN(ci.pen_, CreatePen(PS_GEOMETRIC, 0, color));
	}
	SendMessage(toolbar_->channelSelect()->hWnd(), CB_SETCURSEL, 0, 0);
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

void MultiChartView::selectChannel(int chnId) {
	if (chnId >= 0 && chnId < channelCount_) {
		selectedChannel_ = chnId;
		InvalidateRect(hWnd_, NULL, false);
	}
}

void MultiChartView::draw(LPARAM lParam) {
	int ch = selectedChannel_;
	// get x,y from mouse click
	int cx = GET_X_LPARAM(lParam) + offsetX_;
	int cy = GET_Y_LPARAM(lParam);
	if (cy < MULTICHARTVIEW_TOOLBARHEIGHT) {
		cy = MULTICHARTVIEW_TOOLBARHEIGHT;
	}
	int ix = (int)CHART_FROM_X(cx);
	lastDrawX_ = ix;
	RECT rect;
	SYSPR(GetClientRect(hWnd_, &rect));
	double y = (double)(rect.bottom - cy) / (rect.bottom - rect.top - MULTICHARTVIEW_TOOLBARHEIGHT);
	lastDrawY_ = y;
	int at = 0;
	if (configuration_.canDrawWithoutSelection || selection_.binSearch(&ix, at, Collection::compareInt) != NULL) {
		dataSeries_->set(ix, ch, &y);
	}
	InvalidateRect(hWnd_, NULL, false);
}

void MultiChartView::drawLine(LPARAM lParam) {
	int ch = selectedChannel_;
	// get x of mouse click
	int cx = GET_X_LPARAM(lParam) + offsetX_;
	// get y from data at mouse click
	int cy = GET_Y_LPARAM(lParam);
	if (cy < MULTICHARTVIEW_TOOLBARHEIGHT) {
		cy = MULTICHARTVIEW_TOOLBARHEIGHT;
	}
	int ix = (int)CHART_FROM_X(cx);
	int at = 0;
	RECT rect;
	SYSPR(GetClientRect(hWnd_, &rect));
	double y = (double)(rect.bottom - cy) / (rect.bottom - rect.top - MULTICHARTVIEW_TOOLBARHEIGHT);
	int x1 = lastDrawX_; double y1 = lastDrawY_;
	int x2 = ix; double y2 = y;
	if (x1 > x2) {
		x2 = x1; x1 = ix;
		y2 = y1; y1 = y;
	}
	double m = (y2 - y1) / (x2 - x1);
	while (++x1 < x2) {
		y1 += m;
		if (configuration_.canDrawWithoutSelection || selection_.binSearch(&x1, at, Collection::compareInt) != NULL) {
			lastDrawX_ = x1;
			lastDrawY_ = y1;
			dataSeries_->set(x1, ch, &y1);
		}
	}
	InvalidateRect(hWnd_, NULL, false);
}

void MultiChartView::update() {
	SYSFN(hBackgroundBrush_, CreateSolidBrush(configuration_.backgroundColor));
	DWORD gridColor = addColors(configuration_.backgroundColor, 0x101010);
	SYSFN(hGridBrush_, CreateSolidBrush(gridColor));
	SYSFN(hGridPen_, CreatePen(PS_GEOMETRIC, 0, gridColor));
}

static 	BLENDFUNCTION blendFunction_ = {
	AC_SRC_OVER,
	0,
	20,
	0
};
void MultiChartView::drawChannel(HDC hdc, int chnId, RECT* rect, int dataStart, int dataRange) {
	RECT frameRect;
	CopyRect(&frameRect, rect);
	int width = rect->right - rect->left;
	int height = rect->bottom - rect->top;
	float x = 0;
	CHARTCHANNELINFO& ci = channels_[chnId];

	DWORD color = selectedChannel_ != chnId ? mixColors(ci.color, configuration_.backgroundColor, 0.3f) : ci.color;
	HBRUSH SYSFN(brush, CreateSolidBrush(color));
	HPEN SYSFN(pen, CreatePen(PS_GEOMETRIC, 0, color));
	HBRUSH SYSFN(gridBrush, CreateSolidBrush(addColors(color, 0x404040)));

	HPEN SYSFN(penOld, (HPEN)SelectObject(hdc, pen));
	// paint data points
	int gridX = dataStart % configuration_.grid.x;
	int px = -1, py = 0;
	for (int i = dataStart; i < dataRange; i++) {
		double value[4];
		if (dataSeries_->get(i, chnId, value)) {
			int y = (int)(value[0] * height);
			frameRect.left = (long)x;
			frameRect.right = (long)(x + configuration_.stepSize.x * scaleX_);
			HBRUSH br = brush;
			if (++gridX == configuration_.grid.x) {
				br = gridBrush;
				gridX = 0;
			}
			int cy = rect->bottom - y;
			switch (ci.paintMode) {
			case CHART_PAINTMODE_BAR:
				frameRect.top = cy;
				frameRect.bottom = rect->bottom;
				SYSPR(FillRect(hdc, &frameRect, br));
				break;
			case CHART_PAINTMODE_LINE:
				frameRect.top = cy - 2;
				frameRect.bottom = cy + 3;
				if (px != -1) {
					SYSPR(MoveToEx(hdc, px, py, NULL));
					SYSPR(LineTo(hdc, frameRect.left, cy));
				}
				px = frameRect.right; py = cy;
				SYSPR(FillRect(hdc, &frameRect, br));
				break;
			case CHART_PAINTMODE_BOX:
				RECT rect;
				rect.left = frameRect.left;
				rect.right = frameRect.left + (int)(value[1] * configuration_.stepSize.x * scaleX_);
				rect.top = cy+1 - (int)(configuration_.stepSize.y * scaleY_);
				rect.bottom = cy-1 + (int)(configuration_.stepSize.y * scaleY_);
				SYSPR(FillRect(hdc, &rect, br));
				break;
			case CHART_PAINTMODE_CUSTOM1:
				configuration_.customDraw[0](hdc, &frameRect, i);
				break;
			case CHART_PAINTMODE_CUSTOM2:
				configuration_.customDraw[1](hdc, &frameRect, i);
				break;
			case CHART_PAINTMODE_CUSTOM3:
				configuration_.customDraw[2](hdc, &frameRect, i);
				break;
			case CHART_PAINTMODE_CUSTOM4:
				configuration_.customDraw[3](hdc, &frameRect, i);
				break;
			}
			// overpaint selection
			int ix = 0;
			if (selection_.binSearch(&i, ix, Collection::compareInt)) {
				SYSPR(AlphaBlend(
					hdc, frameRect.left - 1, rect->top, frameRect.right - frameRect.left + 2, height,
					selectHdc_, 0, 0, 1, 1, blendFunction_));
			}
		}
		x += CHART_TO_X(1);
	}

	SelectObject(hdc, penOld);
	DeleteObject(brush);
	DeleteObject(pen);
	DeleteObject(gridBrush);
}

///************************************
/// WINDOW functions and event handlers
///************************************
LRESULT CALLBACK MultiChartView::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		//case WM_SIZE:
		//	onSize(wParam, lParam);
		//	DefWindowProc(hWnd, uMsg, wParam, lParam);
		//	break;
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

LRESULT MultiChartView::onCreate() {
	// create toolbar
	toolbar_ = NEW_(MultiChartViewToolbar, this, 0x2000);
	toolbar_->channelSelect()->setSelection(selectedChannel_);
	return 1;
}

LRESULT MultiChartView::onSize(WPARAM wParam, LPARAM lParam) {
	// resize toolbar
	SYSPR(SetWindowPos(toolbar_->hWnd(), NULL, 0, 0, width_, MULTICHARTVIEW_TOOLBARHEIGHT-3, SWP_SHOWWINDOW));
	if (dataSeries_ != NULL) {
		int maxWidth = (int)CHART_TO_X(dataSeries_->getLength()) + offsetX_;
		maxPosX_ = maxWidth - width_;
		SCROLLINFO si = {
			sizeof(SCROLLINFO),				// cbSize
			SIF_RANGE | SIF_PAGE | SIF_POS,	// fMask
			0,								// nMin
			maxWidth,						// nMax
			(UINT)width_,					// nPage
			offsetX_						// nPos
		};
		SetScrollInfo(hWnd_, SB_HORZ, &si, false);
	}
	// drop hdc to recreate it
	HBITMAP bitmap = (HBITMAP)SelectObject(hdc_, hBitmap_);
	DeleteObject(bitmap);
	DeleteDC(hdc_);
	hdc_ = NULL;
	return 0;
}

void MultiChartView::onScrollX(WPARAM wParam, LPARAM lParam) {
	int posX;

	int pageSize = (int)CHART_TO_X(4);
	int lineSize = (int)CHART_TO_X(1);

	switch (LOWORD(wParam)) {
	case SB_PAGEUP:
		posX = offsetX_ - pageSize;
		break;
	case SB_PAGEDOWN:
		posX = offsetX_ + pageSize;
		break;
	case SB_LINEUP:
		posX = offsetX_ - lineSize;
		break;
	case SB_LINEDOWN:
		posX = offsetX_ + lineSize;
		break;
	case SB_THUMBPOSITION:
		posX = HIWORD(wParam);
		break;
	default:
		posX = offsetX_;
	}

	posX = max(0, posX);
	posX = min(maxPosX_, posX);

	if (posX != offsetX_) {
		offsetX_ = posX;
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
}

void MultiChartView::onLeftUp(WPARAM wParam, LPARAM lParam) {
	isSelecting_ = false;
	this->select();
}

void MultiChartView::onRightDown(WPARAM wParam, LPARAM lParam) {
	if (wParam & MK_SHIFT) {
		drawLine(lParam);
	} else {
		draw(lParam);
	}
}

void MultiChartView::onRightUp(WPARAM wParam, LPARAM lParam) {
}

void MultiChartView::onMouseMove(WPARAM wParam, LPARAM lParam) {
	if (wParam & MK_LBUTTON) {
		long x = GET_X_LPARAM(lParam);
		selectRect_.left = min(dragStartX_, x);
		selectRect_.right = max(dragStartX_, x);
		InvalidateRect(hWnd_, NULL, false);
	}
	if (wParam & MK_RBUTTON) {
		draw(lParam);
	}
}

int MultiChartView::onPaint(HDC hdc, PAINTSTRUCT* ps) {
	// create custom hdc and bitmap
	if (hdc_ == NULL) {
		SYSFN(hdc_, CreateCompatibleDC(hdc));
		HBITMAP SYSFN(hBitmap, CreateCompatibleBitmap(hdc, width_, height_));
		HBITMAP SYSFN(hBitmap_, (HBITMAP)SelectObject(hdc_, hBitmap));
	}

	// create selection hdc and bitmap
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
	int height = height_ - MULTICHARTVIEW_TOOLBARHEIGHT;
	RECT clientRect = { 0, 0, width_, height };
	SYSPR(FillRect(hdc_, &clientRect, hBackgroundBrush_));

	if (dataSeries_ != NULL) {
		// get x range
		int dataStart = (int)CHART_FROM_X(offsetX_);
		int dataRange = (int)CHART_FROM_X(width_) + dataStart + 1;

		// paint horizontal grid lines
		float deltaY = (float)height_ / configuration_.grid.y;
		HPEN SYSFN(hpen, (HPEN)SelectObject(hdc_, hGridPen_));
		for (float y = (float)clientRect.bottom; y > (float)clientRect.top; y -= deltaY) {
			SYSPR(MoveToEx(hdc_, 0, (int)y, NULL));
			SYSPR(LineTo(hdc_, width_, (int)y));
		}
		SYSPR(SelectObject(hdc_, hpen));
		
		// paint vertical grid bars
		RECT frameRect;
		float deltaX = CHART_TO_X(configuration_.grid.x);
		float x = 0;	// (float)(-offsetX_ % (int)(configuration_.grid.x * configuration_.stepSize.x * scaleX_));
		int gridX = dataStart % configuration_.grid.x;
		for (int i = dataStart; i < dataRange; i++) {
			if (++gridX == configuration_.grid.x) {
				frameRect.top = clientRect.top;
				frameRect.bottom = clientRect.bottom;
				frameRect.left = (long)x;
				frameRect.right = (long)(x + configuration_.stepSize.x * scaleX_);
				SYSPR(FillRect(hdc_, &frameRect, hGridBrush_));
				gridX = 0;
			}
			x += CHART_TO_X(1);
		}
		
		// paint chart
		for (int ch = 0; ch < channelCount_; ch++) {
			if (ch != selectedChannel_) {
				drawChannel(hdc_, ch, &clientRect, dataStart, dataRange);
			}
		}
		drawChannel(hdc_, selectedChannel_, &clientRect, dataStart, dataRange);
	}

	// paint selection rectangle
	if (isSelecting_) {
		SYSPR(AlphaBlend(
			hdc_, selectRect_.left, selectRect_.top, selectRect_.right - selectRect_.left, height_,
			selectHdc_, 0, 0, 1, 1, blendFunction_));
	}

	BitBlt(hdc, clientRect.left, MULTICHARTVIEW_TOOLBARHEIGHT, width_, height, hdc_, 0, 0, SRCCOPY);

	return 0;
}

NS_FW_WIN_END