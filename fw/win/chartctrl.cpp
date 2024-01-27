#include "win/chartctrl.h"
#include <math.h>

NS_FW_BASE_USE
NS_FW_WIN_USE

char* ChartCtrl::windowClassName_ = "ChartCtrl";
ATOM ChartCtrl::windowClass_ = 0;

dword scaleColor(dword color, float f) {
	float r = ((byte*)&color)[0] * f;
	float g = ((byte*)&color)[1] * f;
	float b = ((byte*)&color)[2] * f;
	((byte*)&color)[0] = r > 255 ? 255 : (byte)r;
	((byte*)&color)[1] = g > 255 ? 255 : (byte)g;
	((byte*)&color)[2] = b > 255 ? 255 : (byte)b;
	return color;
}

ChartSettings ChartCtrl::defaultSettings_ = {
	{ 16, 16 },				// unit1
	{  4,  4 },				// unit2
	true,					// showGrid
	0x005c3329,				// backgroundColor
	0x0029335c,				// foregroundColor
	0x00b86652,				// grid1Color
	//0x00dc7a62,				// grid2Color
	0x004799ad,				// lineColor
	0x00a3ccd6,				// textColor
	ChartInsertModeFree,	// insertMode
	ChartDragModeFree,		// dragMode
	ChartDrawModeLine,		// drawMode
	 8,  8,					// lineSizeX, lineSizeY
	64, 64,					// pageSizeX, pageSizeY
	10,						// dragSpeed
	0.01f					// scaleSpeed
};

static BLENDFUNCTION blendFunction_ = {
	AC_SRC_OVER,	// BlendOp
	0,				// BlendFlags
	0,				// SourceConstantAlpha
	0				// AlphaFormat
};

ChartCtrl::ChartCtrl() {
	initialize(&ChartCtrl::defaultSettings_);
}

ChartCtrl::ChartCtrl(ChartSettings* settings) {
	initialize(settings);
}

ChartCtrl::~ChartCtrl() {
	HBITMAP bitmap = (HBITMAP)SelectObject(hDC_, hBitmap_);
	DeleteObject(bitmap);
	DeleteDC(hDC_);

	SYSPR(DeleteObject(backgroundBrush_));
	SYSPR(DeleteObject(grid1Pen_));
	SYSPR(DeleteObject(grid2Pen_));
	//SYSPR(DeleteObject(highlightBrush_));
}

char* const ChartCtrl::registerWindowClass() {
	if (ChartCtrl::windowClass_ == 0) {
		ChartCtrl::windowClass_ = Window::registerClass(ChartCtrl::windowClassName_, NULL, 0);
	}
	return ChartCtrl::windowClassName_;
}

void ChartCtrl::initialize(ChartSettings* settings) {
	fmw::memcpy(&settings_, settings, sizeof(ChartSettings));

	//cursor_ = { 0, 0 };
	//points_.init(sizeof(POINT), 16);
	//selected_ = -1;
	//lockedDirection_ = 0;

	dataSource_ = NULL;

	scaleRangeX_[0] = 1.0f;
	scaleRangeX_[1] = 16.0f;
	scaleX_ = 0.0f;
	setScaleX(1.0f);

	scaleRangeY_[0] = 1.0f;
	scaleRangeY_[1] = 16.0f;
	scaleY_ = 0.0f;
	setScaleY(1.0f);

	originalVirtualSize_ = { 0, 0, 0, 0 };

	scrollPosX_ = 0;
	offsetX_ = 0;
	offsetX2_ = 0;
	scrollPosY_ = 0;
	offsetY_ = 0;
	offsetY2_ = 0;

	hDC_ = NULL;
	hBitmap_ = NULL;
	backgroundBrush_ = NULL;
	grid1Pen_ = NULL;
	grid2Pen_ = NULL;

	scrollInfoX_.lineSize = settings->lineSizeX;
	scrollInfoY_.lineSize = settings->lineSizeY;
	scrollInfoX_.pageSize = settings->pageSizeX;
	scrollInfoY_.pageSize = settings->pageSizeY;

	setColors();

	draggingState_ = DRAGGING_INACTIVE;
	//dragVector_[0] = 0.0f;
	//dragVector_[1] = 0.0f;

	cursor_ = { -1, -1 };
	isHighLightVisible_ = false;
	highLightPos_ = { 0, 0 };

	hasSelection_ = false;
	selectionPos1_ = { 0, 0 };
	selectionPos2_ = { 0, 0 };
}

void ChartCtrl::createWindow(Window* parent, char* name, LONG style, DWORD exStyle) {
	Ctrl::createWindow(parent, name, style, exStyle);

	onLeftUp(ChartCtrl::onLeftUpProc);
	onLeftDown(ChartCtrl::onLeftDownProc);
	onRightUp(ChartCtrl::onRightUpProc);
	onRightDown(ChartCtrl::onRightDownProc);
	onMiddleUp(ChartCtrl::onMiddleUpProc);
	onMiddleDown(ChartCtrl::onMiddleDownProc);
	onMouseMove(ChartCtrl::onMouseMoveProc);
	onWheel(ChartCtrl::onWheelProc);
}

void ChartCtrl::setVirtualSize(int width, int height) {
	originalVirtualSize_.right = width;
	originalVirtualSize_.bottom = height;
	Window::setVirtualSize((int)(width * scaleX_), (int)(height * scaleY_));
}

#pragma region State setters
void ChartCtrl::createBackBuffer(RECT& rect) {
	if (rect.right != 0 && rect.bottom != 0) {
		var hDC = GetDC(hWnd_);
		if (hDC_ == NULL) {
			SYSFN(hDC_, CreateCompatibleDC(hDC));
		}
		DeleteObject(hBitmap_);
		HBITMAP SYSFN(hBitmap, CreateCompatibleBitmap(hDC, rect.right, rect.bottom));
		HBITMAP SYSFN(hBitmap_, (HBITMAP)SelectObject(hDC_, hBitmap));
	}
}
void ChartCtrl::setColors() {
	if (backgroundBrush_) {
		SYSPR(DeleteObject(backgroundBrush_));
		SYSPR(DeleteObject(foregroundBrush_));
		SYSPR(DeleteObject(grid1Pen_));
		SYSPR(DeleteObject(grid2Pen_));
	}
	SYSFN(backgroundBrush_, CreateSolidBrush(settings_.backgroundColor));
	SYSFN(foregroundBrush_, CreateSolidBrush(settings_.foregroundColor));
	SYSFN(grid1Pen_, CreatePen(PS_SOLID, 1, settings_.gridColor));
	SYSFN(grid2Pen_, CreatePen(PS_SOLID, 3, settings_.gridColor));
}

int ChartCtrl::setScaleX(float x) {
	int ret = 0;
	if (x < scaleRangeX_[0]) x = scaleRangeX_[0];
	else if (x > scaleRangeX_[1]) x = scaleRangeX_[1];
	if (scaleX_ != x) {
		scaleX_ = x;
		stepX_ = (int)(settings_.unit1.x * scaleX_);
		stepX2_ = stepX_ * settings_.unit2.x;
		updateOffsetX();
		ret = 1;
	}
	return ret;
}
int ChartCtrl::setScaleY(float y) {
	int ret = 0;
	if (y < scaleRangeY_[0]) y = scaleRangeY_[0];
	else if (y > scaleRangeY_[1]) y = scaleRangeY_[1];
	if (scaleY_ != y) {
		scaleY_ = y;
		stepY_ = (int)(settings_.unit1.y * scaleY_);
		stepY2_ = stepY_ * settings_.unit2.y;
		updateOffsetY();
		ret = 1;
	}
	return ret;
}

void ChartCtrl::dataBind(Collection* dataSource, CHART_DATA_GETTER* dataGetter) {
	dataSource_ = dataSource;
	dataGetter_ = dataGetter;
	if (hDC_ != NULL) {
		InvalidateRect(hWnd_, NULL, false);
	}
}
#pragma endregion

#pragma region UI handlers
LRESULT ChartCtrl::startDragging(POINT& pos) {
	SetCapture(hWnd_);
	dragStartPoint_ = pos;
	draggingState_ = DRAGGING_STARTS;
	return 1;
}
LRESULT ChartCtrl::doDragging(POINT& clientPos, POINT& delta, int keys) {
	LRESULT ret = 0;
	if (draggingState_ == DRAGGING_STARTS) {
		draggingState_ = DRAGGING_ACTIVE;
	}
	if (draggingState_ == DRAGGING_ACTIVE) {
		scrollWindow(scrollInfoX_, SB_THUMBTRACK, scrollInfoX_.pos - delta.x, SB_HORZ, false);
		scrollWindow(scrollInfoY_, SB_THUMBTRACK, scrollInfoY_.pos - delta.y, SB_VERT, false);
		updateHighlight(clientPos, false);
		ret = 1;
	}
	return ret;
}
LRESULT ChartCtrl::stopDragging() {
	ReleaseCapture();
	draggingState_ = DRAGGING_INACTIVE;
	return 1;
}

LRESULT ChartCtrl::startSelection(POINT& pos) {
	SYSPR(SetCapture(hWnd_));
	SYSPR(SetTimer(hWnd_, SelectionTimerEvent, 10, &selectionTimerProc));
	selectionPos1_.x = pos.x;
	selectionPos1_.y = pos.y;
	selectionPos2_.x = pos.x;
	selectionPos2_.y = pos.y;
	selectionState_ = DRAGGING_STARTS;
	hasSelection_ = false;
	isHighLightVisible_ = false;
	return 1;
}
LRESULT ChartCtrl::doSelection(POINT& clientPos, POINT& delta, int keys) {
	LRESULT ret = 0;
	if (selectionState_ == DRAGGING_STARTS) {
		selectionState_ = DRAGGING_ACTIVE;
	}
	if (selectionState_ == DRAGGING_ACTIVE) {
		hasSelection_ = true;
		selectionPos2_.x = clientPos.x;
		selectionPos2_.y = clientPos.y;

		if (clientPos.x < 0) {
			scrollWindow(scrollInfoX_, SB_DELTA, clientPos.x / stepX_, SB_HORZ, false);
		}
		else if (clientPos.x > rect_.right) {
			scrollWindow(scrollInfoX_, SB_DELTA, (clientPos.x - rect_.right) / stepX_, SB_HORZ, false);
		}

		if (clientPos.y < 0) {
			// scroll up
			scrollWindow(scrollInfoY_, SB_DELTA, clientPos.y / stepY_, SB_VERT, false);
		}
		else if (clientPos.y > rect_.bottom) {
			// scroll down
			scrollWindow(scrollInfoY_, SB_DELTA, (clientPos.y - rect_.bottom) / stepY_, SB_VERT, false);
		}

		if (selectionPos1_.x < selectionPos2_.x) {
			selectionRect_.left = selectionPos1_.x;
			selectionRect_.right = selectionPos2_.x - selectionPos1_.x;
		}
		else {
			selectionRect_.left = selectionPos2_.x;
			selectionRect_.right = selectionPos1_.x - selectionPos2_.x;
		}

		if (selectionPos1_.y < selectionPos2_.y) {
			selectionRect_.top = selectionPos1_.y;
			selectionRect_.bottom = selectionPos2_.y - selectionPos1_.y;
		}
		else {
			selectionRect_.top = selectionPos2_.y;
			selectionRect_.bottom = selectionPos1_.y - selectionPos2_.y;
		}

		// checkSelection()

		InvalidateRect(hWnd_, NULL, false);
		ret = 1;
	}
	return ret;
}
LRESULT ChartCtrl::stopSelection() {
	KillTimer(hWnd_, SelectionTimerEvent);
	SYSPR(ReleaseCapture());
	selectionState_ = DRAGGING_INACTIVE;
	if (selectionPos1_.x == selectionPos2_.x || selectionPos1_.y == selectionPos2_.y) {
		hasSelection_ = false;
		InvalidateRect(hWnd_, NULL, false);
	}
	isHighLightVisible_ = true;
	return 1;

}

int ChartCtrl::scrollWindow(ScrollInfo& scrollInfo, int mode, int pos, int nBar, bool update) {
	var ret = Window::scrollWindow(scrollInfo, mode, pos, nBar, update);
	nBar == SB_HORZ ? updateOffsetX() : updateOffsetY();
	return ret;
}

void ChartCtrl::updateOffsetX() {
	offsetX_ = -scrollInfoX_.pos % stepX_;
	offsetX2_ = -scrollInfoX_.pos % stepX2_;
}
void ChartCtrl::updateOffsetY() {
	offsetY_ = -scrollInfoY_.pos % stepY_;
	offsetY2_ = -scrollInfoY_.pos % stepY2_;
}

bool ChartCtrl::fromClientPoint(POINT* clientPos, POINT* pos) {
	var isChanged = false;
	if (pos == NULL) pos = &cursor_;
	var x = (clientPos->x + scrollInfoX_.pos) / stepX_;
	var y = (clientPos->y + scrollInfoY_.pos) / stepY_;
	if (x != pos->x) {
		pos->x = x;
		isChanged = true;
	}
	if (y != pos->y) {
		pos->y = y;
		isChanged = true;
	}
	return isChanged;
}
void ChartCtrl::updateHighlight(POINT& pos, bool invalidate) {
	highLightPos_.x = ((pos.x - offsetX_) / stepX_) * stepX_ + offsetX_;
	highLightPos_.y = ((pos.y - offsetY_) / stepY_) * stepY_ + offsetY_;
	if (invalidate) {
		InvalidateRect(hWnd_, NULL, false);
	}
	isHighLightVisible_ = true;
}

LRESULT ChartCtrl::scale(float f, POINT& screenPos, int keys) {
	var ret = 0;
	var stepX = stepX_, stepY = stepY_;
	if (!(keys & MK_SHIFT)) {
		ret = setScaleX(scaleX_ + f * settings_.scaleSpeed);
	}
	if (!(keys & MK_CONTROL)) {
		ret += setScaleY(scaleY_ + f * settings_.scaleSpeed);
	}
	if (ret) {
		POINT clientPos = { screenPos.x, screenPos.y };
		SYSPR(ScreenToClient(hWnd_, &clientPos));
		var sx = cursor_.x * stepX_ + (stepX_>>1) - clientPos.x;
		var sy = cursor_.y * stepY_ + (stepY_>>1) - clientPos.y;
		LOG("sc: (%d, %d), ps: (%d, %d)\n", sx, sy, clientPos.x, clientPos.y);
		Window::setVirtualSize((int)(originalVirtualSize_.right * scaleX_), (int)(originalVirtualSize_.bottom * scaleY_));
		scrollWindow(scrollInfoX_, SB_THUMBTRACK, sx, SB_HORZ, false);
		scrollWindow(scrollInfoY_, SB_THUMBTRACK, sy, SB_VERT, false);
		updateHighlight(clientPos, false);
	}
	return 1;
}

void ChartCtrl::focusOn(POINT& clientPos) {
	fromClientPoint(&clientPos);
	// center on cursor
	scrollWindow(scrollInfoX_, SB_THUMBTRACK, (cursor_.x * stepX_ + (stepX_>>1)) - (rect_.right>>1), SB_HORZ, false);
	scrollWindow(scrollInfoY_, SB_THUMBTRACK, (cursor_.y * stepY_ + (stepY_>>1)) - (rect_.bottom>>1), SB_VERT, false);
}
#pragma endregion

#pragma region System event handlers
LRESULT ChartCtrl::onCreated() {
	createBackBuffer(rect_);

	// create selection hdc and bitmap
	var hDC = GetDC(hWnd_);
	SYSFN(hSelectionDC_, CreateCompatibleDC(hDC));
	SYSFN(hSelectionBitmap_, CreateCompatibleBitmap(hDC, 1, 1));
	HBRUSH SYSFN(selectColor, CreateSolidBrush(0xFFFFFF));
	SYSPR(SelectObject(hSelectionDC_, hSelectionBitmap_));
	RECT rect = { 0,0,1,1 };
	FillRect(hSelectionDC_, &rect, selectColor);
	DeleteObject(selectColor);
	return 1;
}

LRESULT ChartCtrl::onHScroll(WPARAM wParam, LPARAM lParam) {
	var ret = Window::onHScroll(wParam, lParam);
	updateOffsetX();
	return ret;
}

LRESULT ChartCtrl::onVScroll(WPARAM wParam, LPARAM lParam) {
	var ret = Window::onVScroll(wParam, lParam);
	updateOffsetY();
	return ret;
}

LRESULT ChartCtrl::onSize(RECT& rect, WPARAM state) {
	var res = Window::onSize(rect, state);
	createBackBuffer(rect);
	//setScrolling();
	return res;
}

LRESULT ChartCtrl::onPaint() {
	PAINTSTRUCT ps;
	var SYSFN(hDC, BeginPaint(hWnd_, &ps));

	var width = min(rect_.right, virtualRect_.right - scrollInfoX_.pos);
	var height = min(rect_.bottom, virtualRect_.bottom - scrollInfoY_.pos);
	SYSPR(FillRect(hDC_, &rect_, GetSysColorBrush(COLOR_BACKGROUND)));
	RECT rect{ 0, 0, width, height };
	SYSPR(FillRect(hDC_, &rect, backgroundBrush_));
	
	// draw data points
	int startX = 0, endX = 0;
	int startY = 0, endY = 0;
	if (dataSource_ != NULL) {
		startX = scrollInfoX_.pos / stepX_;
		startY = scrollInfoY_.pos / stepY_;
		endX = (scrollInfoX_.pos + rect_.right) / stepX_;
		endY = (scrollInfoY_.pos + rect_.bottom) / stepY_;
		POINT p;
		for (var x = startX; x < endX; x++) {
			if (dataGetter_(x, startY, endY, &p, dataSource_)) {
				p.x = p.x * stepX_ - scrollInfoX_.pos;
				p.y = p.y * stepY_ - scrollInfoY_.pos;
				rect.left = p.x;
				rect.top = p.y;
				rect.right = p.x + stepX_;
				rect.bottom = p.y + stepY_;
				FillRect(hDC_, &rect, foregroundBrush_);
			}
		}
	}

	// draw highlight
	if (isHighLightVisible_) {
		blendFunction_.SourceConstantAlpha = 40;
		rect = { highLightPos_.x, highLightPos_.y, highLightPos_.x + stepX_, highLightPos_.y + stepY_ };
		SYSPR(AlphaBlend(hDC_, rect.left, rect.top, stepX_, stepY_, hSelectionDC_, 0, 0, 1, 1, blendFunction_));
	}

	if (settings_.showGrid) {
		POINT point;
		SYSPR(SelectObject(hDC_, grid1Pen_));
		for (var x1 = offsetX_; x1 < width; x1 += stepX_) {
			SYSPR(MoveToEx(hDC_, x1, 0, &point));
			SYSPR(LineTo(hDC_, x1, height));
		}
		for (var y1 = offsetY_; y1 < height; y1 += stepY_) {
			SYSPR(MoveToEx(hDC_, 0, y1, &point));
			SYSPR(LineTo(hDC_, width, y1));
		}

		SYSPR(SelectObject(hDC_, grid2Pen_));
		for (var x1 = offsetX2_ - 1; x1 < width; x1 += stepX2_) {
			SYSPR(MoveToEx(hDC_, x1, 0, &point));
			SYSPR(LineTo(hDC_, x1, height));
		}

		for (var y1 = offsetY2_ - 1; y1 < height; y1 += stepY2_) {
			SYSPR(MoveToEx(hDC_, 0, y1, &point));
			SYSPR(LineTo(hDC_, width, y1));
		}
	}

	
	// draw selection
	if (hasSelection_) {
		blendFunction_.SourceConstantAlpha = 20;
		SYSPR(AlphaBlend(hDC_, selectionRect_.left, selectionRect_.top, selectionRect_.right, selectionRect_.bottom, hSelectionDC_, 0, 0, 1, 1, blendFunction_));
	}

	SYSPR(SetBkMode(hDC_, TRANSPARENT));
	SYSPR(SetTextColor(hDC_, settings_.textColor));
	char text[32];
	//wsprintf(text, "%4d|%4d", highLightPos_.x, highLightPos_.y);
	wsprintf(text, "%4d|%4d", cursor_.x, cursor_.y);
	//wsprintf(text, "%4d|%4d", scrollInfoX_.pos, scrollInfoY_.pos);
	SYSPR(TextOut(hDC_, 1, 1, &text[0], 9));

	BitBlt(hDC, 0, 0, rect_.right, rect_.bottom, hDC_, 0, 0, SRCCOPY);

	//// draw grid
	//d = scrollInfoY_.pos / stepY_;
	//var y1 = -(d - (int)d) * stepY_;
	//d /= settings_.unit2.y;
	//float y2 = (d - (int)d) * settings_.unit2.y;
	//SYSPR(SelectObject(hDC, grid1Pen_));
	//for (; y1 < rect_.bottom; y1 += stepY_) {
	//	if (y2++ < settings_.unit2.y) {
	//		SYSPR(SelectObject(hDC, grid1Pen_));
	//	}
	//	else {
	//		y2 -= settings_.unit2.y;
	//		SYSPR(SelectObject(hDC, grid2Pen_));
	//	}
	//	SYSPR(MoveToEx(hDC, 0, (int)y1, &point));
	//	SYSPR(LineTo(hDC, rect_.right, (int)y1));
	//}

	SYSPR(EndPaint(hWnd_, &ps));
	return 0;
}

void ChartCtrl::selectionTimerProc(HWND hWnd, UINT uElapse, UINT_PTR nIDEvent, DWORD unnamedParam4) {
	var SYSFN(ctrl, (ChartCtrl*)GetWindowLongPtr(hWnd, GWLP_USERDATA));
	POINT dummy;
	ctrl->doSelection(ctrl->clientPos_, dummy, ctrl->keys_);
}
#pragma endregion

#pragma region Mouse event handlers
LRESULT ChartCtrl::onLeftUpProc(Window* wnd, POINT& pos, WPARAM wParam) {
	var ctrl = (ChartCtrl*)wnd;
	var ret = ctrl->stopSelection();
	return ret;
}
LRESULT ChartCtrl::onLeftDownProc(Window* wnd, POINT& pos, WPARAM wParam) {
	var ctrl = (ChartCtrl*)wnd;
	var ret = ctrl->startSelection(pos);
	return ret;
}
LRESULT ChartCtrl::onRightUpProc(Window* wnd, POINT& pos, WPARAM wParam) {
	LRESULT ret = 1;
	return ret;
}
LRESULT ChartCtrl::onRightDownProc(Window* wnd, POINT& pos, WPARAM wParam) {
	LRESULT ret = 1;
	return ret;
}
LRESULT ChartCtrl::onMiddleUpProc(Window* wnd, POINT& pos, WPARAM wParam) {
	var ctrl = (ChartCtrl*)wnd;
	if (ctrl->draggingState_ != DRAGGING_ACTIVE) {
		ctrl->focusOn(pos);
	}
	return ctrl->stopDragging();
}
LRESULT ChartCtrl::onMiddleDownProc(Window* wnd, POINT& pos, WPARAM wParam) {
	var ctrl = (ChartCtrl*)wnd;
	return ctrl->startDragging(pos);
}
LRESULT ChartCtrl::onMouseMoveProc(Window* wnd, POINT& clientPos, POINT& delta, WPARAM wParam) {
	var ctrl = (ChartCtrl*)wnd;
	ctrl->keys_ = GET_KEYSTATE_WPARAM(wParam);
	ctrl->clientPos_ = clientPos;
	LRESULT ret = ctrl->doDragging(clientPos, delta, ctrl->keys_);
	ret = ctrl->doSelection(clientPos, delta, ctrl->keys_);
	if (!ret) {
		if (ctrl->fromClientPoint(&clientPos)) {
			ctrl->updateHighlight(clientPos, true);
		}
	}
	return 1;
}
LRESULT ChartCtrl::onWheelProc(Window* wnd, POINT& pos, WPARAM wParam) {
	var ctrl = (ChartCtrl*)wnd;
	int keys = GET_KEYSTATE_WPARAM(wParam);
	float delta = (float)GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
	return ctrl->scale(delta, pos, keys);
}
#pragma endregion