#define _USE_MATH_DEFINES

#include <math.h>
#include "player/src/input-ctrl.h"
#include <base/string.h>

HFONT InputCtrl::smallFont_ = NULL;
HFONT InputCtrl::mediumFont_ = NULL;
HFONT InputCtrl::largeFont_ = NULL;
char* InputCtrl::windowClassName_ = "InputCtrl";
ATOM InputCtrl::windowClass_ = 0;

HBITMAP bitmapVPot1_ = NULL;
HDC backBuffer_ = NULL;
DWORD defaultColors[] = { 0x00400000, 0x00c08060, 0x00804030, 0x00ffffff };

#define GAP 2

InputCtrl::InputCtrl() {
	setColors(defaultColors);
	memset((byte*)&labelSize_, 0, sizeof(SIZE) * 2);
	type_ = InputCtrlType::HPotmeter;
	input_ = NULL;
	isCapturing_ = false;
	showLabel_ = true;
	showValue_ = true;
	font_ = InputCtrl::mediumFont_;
	size_ = 120;

	backBuffer_ = NULL;
	bitmapVPot1_ = NULL;

	mouseCounter_ = 0;
	mouseSpeed1_ = 1;
	mouseSpeed2_ = 5;
	mouseSpeed3_ = 10;
}

InputCtrl::~InputCtrl() {
}

void InputCtrl::cleanup() {
	SYSPR(DeleteObject(InputCtrl::smallFont_));
	SYSPR(DeleteObject(InputCtrl::mediumFont_));
	SYSPR(DeleteObject(InputCtrl::largeFont_));
}

char* const InputCtrl::registerWindowClass() {
	if (InputCtrl::windowClass_ == 0) {
		InputCtrl::windowClass_ = Window::registerClass(InputCtrl::windowClassName_, NULL, 0);
		InputCtrl::smallFont_ = CreateFont(12, 0, 0, 0, 100, false, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Consolas");
		InputCtrl::mediumFont_ = CreateFont(14, 0, 0, 0, 400, false, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial");
		InputCtrl::largeFont_ = CreateFont(18, 0, 0, 0, 800, false, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial");
	}
	return InputCtrl::windowClassName_;
}

void InputCtrl::create(Window* parent, char* name/*, LONG style, DWORD exStyle*/ ) {
	Ctrl::create(parent, "", WS_CHILD | WS_VISIBLE);
	label(name);
	onLeftDown(InputCtrl::onLeftDownProc);
	onLeftUp(InputCtrl::onLeftUpProc);
	onRightUp(InputCtrl::onRightUpProc);
	onMouseMove(InputCtrl::onMouseMoveProc);
}

int InputCtrl::setter(void* obj, Value value, void* unused) {
	var potCtrl = (InputCtrl*)obj;
	SYSPR(InvalidateRect(potCtrl->hWnd_, NULL, false));
	//return PotBase::setter(potCtrl->input_, value);
	//return potCtrl->oldSet_(value);
	return 1;
}

LRESULT InputCtrl::onCreated() {
	//var hdc = GetDC(hWnd_);
	//size_ = 100;
	//ReleaseDC(hWnd_, hdc);
	//if (potV_ == NULL) {
	//	// load PNG from resource
	//	potV_ = LoadBitmap(hInstance_, MAKEINTRESOURCE(IDB_POT));
	//}

	return 0;
}

void InputCtrl::label(TCHAR* lbl) {
	fmw::strncpy(label_, 32, lbl);
	setSize(size_);
}

void InputCtrl::setColors(DWORD* colors) {
	// background, foreground, frame, text
	if (backgroundBrush_ != NULL) {
		SYSPR(DeleteObject(backgroundBrush_));
		SYSPR(DeleteObject(foregroundBrush_));
		SYSPR(DeleteObject(frameBrush_));
		SYSPR(DeleteObject(pen_));
	}
	SYSFN(backgroundBrush_, CreateSolidBrush(colors[0]));
	SYSFN(foregroundBrush_, CreateSolidBrush(colors[1]));
	SYSFN(frameBrush_, CreateSolidBrush(colors[2]));
	SYSFN(pen_, CreatePen(PS_SOLID, 1, colors[2]));
	textColor_ = colors[3];
}

void InputCtrl::setSize(int size) {
	HDC hdc;
	RECT rect;
	size_ = size;
	switch (type_) {
	case InputCtrlType::Knob:
		levelSize_.cx = levelSize_.cy = size >> 2;
		break;
	case InputCtrlType::HPotmeter:
		rect.top = 0;
		levelSize_.cx = size;
		hdc = GetDC(hWnd_);
		SelectObject(hdc, InputCtrl::font_);
		DrawText(hdc, "|", -1, &rect, DT_SINGLELINE | DT_CALCRECT);
		ReleaseDC(hWnd_, hdc);
		levelSize_.cy = rect.bottom;
		break;
	case InputCtrlType::VPotmeter:
		levelSize_.cx = size >> 2;
		levelSize_.cy = size;
		break;
	}
	if (size < 100) {
		font_ = InputCtrl::smallFont_;
		fontSize_ = 12;
	}
	else if (size > 140) {
		font_ = InputCtrl::largeFont_;
		fontSize_ = 14;
	}
	else {
		font_ = InputCtrl::mediumFont_;
		fontSize_ = 16;
	}

	// get label rect
	if (showLabel_ || label_ == NULL) {
		RECT rect;
		memset(&rect, 0, sizeof(RECT));
		var hdc = GetDC(hWnd_);
		SelectObject(hdc, InputCtrl::font_);
		DrawText(hdc, (TCHAR*)&label_, -1, &rect, DT_SINGLELINE | DT_CALCRECT);
		ReleaseDC(hWnd_, hdc);
		labelSize_.cx = rect.right;
		labelSize_.cy = rect.bottom;
	}
	else {
		labelSize_.cx = 0;
		labelSize_.cy = 0;
	}

	updateClientRect();
}

void InputCtrl::updateClientRect(bool resize) {
	// label - labelSize_.cy
	// level - size
	// value - labelSize_.cy
	rect_.left = 0;
	rect_.top = 0;
	switch (type_) {
	case InputCtrlType::Knob:
		rect_.right = levelSize_.cx + 2 * GAP;
		rect_.bottom = GAP + labelSize_.cy + GAP + levelSize_.cy + GAP + labelSize_.cy + GAP;
		break;
	case InputCtrlType::HPotmeter:
		rect_.right = GAP + labelSize_.cx + GAP + levelSize_.cx + GAP;
		rect_.bottom = GAP + levelSize_.cy + GAP;
		break;
	case InputCtrlType::VPotmeter:
		rect_.right = levelSize_.cx + 2 * GAP;
		rect_.bottom = GAP + labelSize_.cy + GAP + levelSize_.cy + GAP + labelSize_.cy + GAP;
		break;
	}

	if (resize) {
		set(-1, 0, rect_.right, rect_.bottom);
	}
}

void InputCtrl::type(InputCtrlType t) {
	type_ = t;
	setSize(size_);
}

void InputCtrl::input(Input* p) {
	input_ = p;
	// calculate angle
	var v = input_->getNormalized();
	angle_ = (1.0f - v) * 300.0f - 60.0f;
	//oldSet_.obj = input_->set.obj;
	input_->set.add(this, &InputCtrl::setter);
}

LRESULT InputCtrl::onLeftDownProc(Window* wnd, POINT& pos, WPARAM state) {
	var ctrl = (InputCtrl*)wnd;
	ctrl->isCapturing_ = true;
	SYSPR(SetCapture(ctrl->hWnd_));
	return 0;
}

LRESULT InputCtrl::onLeftUpProc(Window* wnd, POINT& pos, WPARAM state) {
	var ctrl = (InputCtrl*)wnd;
	ctrl->isCapturing_ = false;
	SYSPR(ReleaseCapture());
	return 0;
}

LRESULT InputCtrl::onRightUpProc(Window* wnd, POINT& pos, WPARAM state) {
	var ctrl = (InputCtrl*)wnd;
	return 0;
}

LRESULT InputCtrl::onMouseMoveProc(Window* wnd, POINT& pos, POINT& delta, WPARAM state) {
	var potCtrl = (InputCtrl*)wnd;
	if (potCtrl->isCapturing_ && (state & MK_LBUTTON)) {
		int count = potCtrl->mouseSpeed1_;
		if (state & MK_CONTROL) count *= potCtrl->mouseSpeed2_;
		if (state & MK_SHIFT) count *= potCtrl->mouseSpeed3_;
		switch (potCtrl->type_) {
		case InputCtrlType::Knob:
			//var cx = potCtrl->rect().right / 2.0f, cy = potCtrl->rect().bottom / 2.0f;
			//var angle = atan2(pos.x - cx, pos.y - cy);
			//break;
		case InputCtrlType::HPotmeter:
			potCtrl->mouseCounter_ += delta.x;
			break;
		case InputCtrlType::VPotmeter:
			potCtrl->mouseCounter_ += delta.y;
			break;
		}

		if (potCtrl->mouseCounter_ < -10) {
			potCtrl->mouseCounter_ = 0;
			potCtrl->input_->dec(count);
		}
		else if (potCtrl->mouseCounter_ > 10) {
			potCtrl->mouseCounter_ = 0;
			potCtrl->input_->inc(count);
		}
		//InvalidateRect(potCtrl->hWnd_, NULL, true);
	}
	return 0;
}

void InputCtrl::drawHPotmeter(HDC hdc, float value) {
	var x = GAP, y = GAP;
	// draw label
	if (showLabel_) {
		var cy = ((rect_.bottom - labelSize_.cy) >> 1);
		TextOut(hdc, x, cy, label_, fmw::strlen(label_));
		x += labelSize_.cx + GAP;
	}

	// draw level
	RECT rect = { x, GAP, x + levelSize_.cx, GAP + levelSize_.cy };
	FrameRect(hdc, &rect, frameBrush_);
	rect.right = x + (int)floor(value * levelSize_.cx);
	FillRect(hdc, &rect, foregroundBrush_);

	// draw value
	if (showValue_) {
		RECT valueRect;
		memset(&valueRect, 0, sizeof(RECT));
		DrawText(hdc, (TCHAR*)&valueText_, -1, &valueRect, DT_SINGLELINE | DT_CALCRECT);
		x += (levelSize_.cx - valueRect.right) >> 1;
		TextOut(hdc, x, GAP, valueText_, fmw::strlen(valueText_));
	}
}

void InputCtrl::drawVPotmeter(HDC hdc, float value) {
	var x = GAP, y = GAP;
	// draw label
	if (showLabel_) {
		var cx = (rect_.right - labelSize_.cx) >> 1;
		TextOut(hdc, cx, GAP, label_, fmw::strlen(label_));
		y += labelSize_.cy + GAP;
	}

	// draw level
	RECT rect = { x, y, x + levelSize_.cx, y + levelSize_.cy };
	FrameRect(hdc, &rect, frameBrush_);
	rect.top = y + (int)floor((1.0f - value) * levelSize_.cy);
	FillRect(hdc, &rect, foregroundBrush_);
	y += levelSize_.cy + GAP;

	// draw value
	if (showValue_) {
		RECT valueRect;
		memset(&valueRect, 0, sizeof(RECT));
		DrawText(hdc, (TCHAR*)&valueText_, -1, &valueRect, DT_SINGLELINE | DT_CALCRECT);
		x = (rect_.right - valueRect.right) >> 1;
		TextOut(hdc, x, y, valueText_, fmw::strlen(valueText_));
	}
}

void InputCtrl::drawKnob(HDC hdc, int& x, int& y, float value) {
	// draw level
	SelectObject(hdc, foregroundBrush_);
	//SelectObject(hdc, frameBrush_);
	SelectObject(hdc, pen_);
	Ellipse(hdc, GAP, y, GAP + levelSize_.cx, y + levelSize_.cy);
	// 0.0 -> 240
	// 0.5 ->  90
	// 1.0 -> -60
	var th = -M_PI / 180.0f * (240.0f - value * 300.0f);
	var rd = levelSize_.cx >> 1;
	var cx = GAP + rd;
	var cy = GAP + rd;
	if (showLabel_) cy += GAP + labelSize_.cy;
	MoveToEx(hdc, cx, cy, (LPPOINT)NULL);
	cx += (int)(rd * cos(th)); cy += (int)(rd * sin(th));
	//SelectObject(hdc, pen_);
	LineTo(hdc, cx, cy);
	y += levelSize_.cy + GAP;
}

void InputCtrl::drawNumber(HDC hdc, int& x, int& y, float value) {
	// draw value
	RECT valueRect;
	memset(&valueRect, 0, sizeof(RECT));
	DrawText(hdc, (TCHAR*)&valueText_, -1, &valueRect, DT_SINGLELINE | DT_CALCRECT);
	var cx = (rect_.right - valueRect.right) >> 1;
	TextOut(hdc, cx, y, valueText_, fmw::strlen(valueText_));
	y += valueRect.bottom;
}

LRESULT InputCtrl::onPaint() {
	PAINTSTRUCT ps;
	var hdc = BeginPaint(hWnd_, &ps);
	var y = GAP, x = GAP;
	// draw background
	RECT bgRect = { 0, 0, rect_.right, fontSize_ };
	FillRect(hdc, &bgRect, backgroundBrush_);
	SelectObject(hdc, InputCtrl::font_);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, textColor_);
	// draw label
	if (showLabel_) {
		var cx = ((rect_.right - labelSize_.cx) >> 1);
		TextOut(hdc, cx, 0, label_, fmw::strlen(label_));
		y += labelSize_.cy + GAP;
	}
	var value = input_->getNormalized();
	if (showValue_) input_->getValueAsString(valueText_, 32);
	switch (type_) {
	case InputCtrlType::HPotmeter: drawHPotmeter(hdc, value); break;
	case InputCtrlType::VPotmeter: drawVPotmeter(hdc, value); break;
	case InputCtrlType::Knob: drawKnob(hdc, x, y, value); break;
	case InputCtrlType::Number: drawNumber(hdc, x, y, value); break;
	}

	// draw value
	if (showValue_) {
		RECT bgRect = { 0, y, rect_.right, rect_.bottom };
		RECT valueRect;
		memset(&valueRect, 0, sizeof(RECT));
		DrawText(hdc, (TCHAR*)&valueText_, -1, &valueRect, DT_SINGLELINE | DT_CALCRECT);
		FillRect(hdc, &bgRect, backgroundBrush_);
		x = (rect_.right - valueRect.right) >> 1;
		TextOut(hdc, x, y, valueText_, fmw::strlen(valueText_));
	}

	EndPaint(hWnd_, &ps);
	//ValidateRect(hWnd_, NULL);
	return 0;
}

LRESULT InputCtrl::onEraseBkgnd(HDC hDC) {
	return 1;
}
