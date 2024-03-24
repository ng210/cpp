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
// background, foreground, frame, text
DWORD defaultColors[] = { 0x00400000, 0x00c08060, 0x00804030, 0x00ffffff };

InputCtrl::InputCtrl() {
	//frameBrush_ = NULL;
	backgroundBrush_ = NULL;
	foregroundBrush_ = NULL;
	setColors(defaultColors);
	type_ = InputCtrlType::HPotmeter;
	input_ = NULL;
	isCapturing_ = false;
	showLabel_ = true;
	showValue_ = true;
	font_ = NULL;
	size_ = 100;
	gap = 2;

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
}

char* const InputCtrl::registerWindowClass() {
	if (InputCtrl::windowClass_ == 0) {
		WNDCLASSEX wndClassEx = {
			sizeof(WNDCLASSEX),				// cbSize;
			CS_PARENTDC,					// style;
			&Window::wndProcWrapper,		// lpfnWndProc;
			0,                              // cbClsExtra;
			0,                              // cbWndExtra;
			NULL,							// hInstance;
			NULL,                           // hIcon;
			LoadCursor(NULL, IDC_HAND),		// hCursor;
			NULL,							// hbrBackground;
			NULL,                           // lpszMenuName;
			InputCtrl::windowClassName_,	// lpszClassName;
			NULL                            // hIconSm;
		};
		InputCtrl::windowClass_ = Window::registerClass(wndClassEx);
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
	setScale(size_);
}

void InputCtrl::setColors(DWORD* colors) {
	// background, foreground, frame, text
	if (backgroundBrush_ != NULL) {
		SYSPR(DeleteObject(backgroundBrush_));
		SYSPR(DeleteObject(foregroundBrush_));
		//SYSPR(DeleteObject(frameBrush_));
		SYSPR(DeleteObject(pen_));
	}
	SYSFN(backgroundBrush_, CreateSolidBrush(colors[0]));
	SYSFN(foregroundBrush_, CreateSolidBrush(colors[1]));
	//SYSFN(frameBrush_, CreateSolidBrush(colors[2]));
	SYSFN(pen_, CreatePen(PS_SOLID, 1, colors[2]));
	textColor_ = colors[3];
}

void InputCtrl::setScale(int size) {
	// size -> font height
	// 0 -> 8
	// 100 -> 14
	// 200 -> 20
	var fh = 8 + (int)(6.0f * size / 100.0f);
	if (fontSize_ != fh) {
		if (font_) DeleteObject(font_);
		font_ = CreateFont(fh, (int)(0.4f * fh), 0, 0, 100, false, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial");

		fontSize_ = fh;
	}
	size_ = size;
	var wi = gap + (int)(8.0f * fontSize_ / 4.0f) + gap;

	int x = 0, y = 0;
	if (showLabel_) {
		labelRect_ = { 0, y, wi, fontSize_ };
		y += labelRect_.bottom + gap;
	}

	switch (type_) {
		case InputCtrlType::Knob: {
			x = 2 + 2 * gap;
			levelRect_ = { x, y, x + wi - 2 * x, y + wi - 2 * x };
			break;
		}
		case InputCtrlType::HPotmeter:{
			var h = fontSize_;
			levelRect_ = { x, y, x + (size_ >> 1), y + h };
			break;
		}
		case InputCtrlType::VPotmeter: {
			var w = fontSize_ >> 1;
			x = (wi - w) >> 1;
			levelRect_ = { x, y, x + w, y + (size_ >> 1) };
			break;
		}
	}

	y = levelRect_.bottom + gap;
	if (showValue_) {
		if (type_ != InputCtrlType::HPotmeter) {
			valueRect_ = { 0, y, wi, y + fontSize_ };
		}
		else {
			valueRect_ = { levelRect_.left, levelRect_.top, levelRect_.right, levelRect_.bottom };
			//fmw::memcpy(&valueRect_, &levelRect_, sizeof(RECT));
		}
		y = valueRect_.bottom;
	}

	setSize(wi, y);
}

void InputCtrl::type(InputCtrlType t) {
	type_ = t;
}

void InputCtrl::input(InputBase* p) {
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
			potCtrl->mouseCounter_ -= delta.y;
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
	FillRect(hdc, &levelRect_, backgroundBrush_);
	var v = input_->getNormalized();
	var x = levelRect_.left + (int)(v * (levelRect_.right - levelRect_.left));
	var wi = (levelRect_.bottom - levelRect_.top) >> 1;
	RECT rect = { levelRect_.left, levelRect_.top, x, levelRect_.bottom };
	FillRect(hdc, &rect, foregroundBrush_);

	//var x = 1, y = 1;
	//// draw label
	//if (showLabel_) {
	//	var cy = ((rect_.bottom - labelSize_.cy) >> 1);
	//	TextOut(hdc, x, cy, label_, fmw::strlen(label_));
	//	x += labelSize_.cx + 1;
	//}

	//// draw level
	//RECT rect = { x, 1, x + levelSize_.cx, 1 + levelSize_.cy };
	//FrameRect(hdc, &rect, frameBrush_);
	//rect.right = x + (int)floor(value * levelSize_.cx);
	//FillRect(hdc, &rect, foregroundBrush_);

	//// draw value
	//if (showValue_) {
	//	RECT valueRect;
	//	memset(&valueRect, 0, sizeof(RECT));
	//	DrawText(hdc, (TCHAR*)&valueText_, -1, &valueRect, DT_SINGLELINE | DT_CALCRECT);
	//	x += (levelSize_.cx - valueRect.right) >> 1;
	//	TextOut(hdc, x, 1, valueText_, fmw::strlen(valueText_));
	//}
}

void InputCtrl::drawVPotmeter(HDC hdc, float value) {
	FillRect(hdc, &levelRect_, backgroundBrush_);
	var v = input_->getNormalized();
	var y = levelRect_.top + (int)((1.0f - v) * (levelRect_.bottom - levelRect_.top));
	var he = (levelRect_.right - levelRect_.left) >> 1;
	RECT rect = { levelRect_.left, y, levelRect_.right, levelRect_.bottom };
	FillRect(hdc, &rect, foregroundBrush_);
}

void InputCtrl::drawKnob(HDC hdc, float value) {
	SelectObject(hdc, foregroundBrush_);
	SelectObject(hdc, pen_);
	Ellipse(hdc, levelRect_.left, levelRect_.top, levelRect_.right, levelRect_.bottom);
	// 0.0 -> 240
	// 0.5 ->  90
	// 1.0 -> -60
	var th = -M_PI / 180.0f * (240.0f - value * 300.0f);
	var wi = levelRect_.right - levelRect_.left;
	var rd = wi >> 1;
	var cx = levelRect_.left + rd;
	var cy = levelRect_.top + rd;
	MoveToEx(hdc, cx, cy, (LPPOINT)NULL);
	cx += (int)(rd * cos(th));
	cy += (int)(rd * sin(th));
	LineTo(hdc, cx, cy);
}

void InputCtrl::drawNumber(HDC hdc, float value) {
	// draw value
	//FillRect(hdc, &levelRect_, backgroundBrush_);
	input_->getValueAsString(valueText_, 32);
	DrawText(hdc, valueText_, -1, &levelRect_, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

LRESULT InputCtrl::onPaint() {
	PAINTSTRUCT ps;
	var hdc = BeginPaint(hWnd_, &ps);
	//FrameRect(hdc, &rect_, foregroundBrush_);

	SelectObject(hdc, InputCtrl::font_);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, textColor_);

	if (showLabel_) {
		// draw label
		FillRect(hdc, &labelRect_, backgroundBrush_);
		SYSPR(DrawText(hdc, label_, -1, &labelRect_, DT_CENTER | DT_SINGLELINE));
	}
	var value = input_->getNormalized();
	input_->getValueAsString(valueText_, 32);

	switch (type_) {
		case InputCtrlType::HPotmeter: drawHPotmeter(hdc, value); break;
		case InputCtrlType::VPotmeter: drawVPotmeter(hdc, value); break;
		case InputCtrlType::Knob: drawKnob(hdc, value); break;
		case InputCtrlType::Number: drawNumber(hdc, value); break;
	}

	if (showValue_) {
		if (type_ != InputCtrlType::HPotmeter) {
			FillRect(hdc, &valueRect_, backgroundBrush_);
		}
		DrawText(hdc, valueText_, -1, &valueRect_, DT_CENTER | DT_SINGLELINE);
	}


	//// draw background
	//RECT bgRect = { 0, 0, rect_.right, fontSize_ };
	//FillRect(hdc, &bgRect, backgroundBrush_);
	//SelectObject(hdc, InputCtrl::font_);
	//SetBkMode(hdc, TRANSPARENT);
	//SetTextColor(hdc, textColor_);
	//// draw label
	//if (showLabel_) {
	//	var cx = ((rect_.right - labelSize_.cx) >> 1);
	//	TextOut(hdc, cx, 0, label_, fmw::strlen(label_));
	//	y += labelSize_.cy + 1;
	//}
	//var value = input_->getNormalized();
	//if (showValue_) input_->getValueAsString(valueText_, 32);

	//switch (type_) {
	//case InputCtrlType::HPotmeter: drawHPotmeter(hdc, value); break;
	//case InputCtrlType::VPotmeter: drawVPotmeter(hdc, value); break;
	//case InputCtrlType::Knob: drawKnob(hdc, x, y, value); break;
	//case InputCtrlType::Number: drawNumber(hdc, x, y, value); break;
	//}

	//// draw value
	//if (showValue_) {
	//	RECT bgRect = { 0, y, rect_.right, rect_.bottom };
	//	RECT valueRect;
	//	memset(&valueRect, 0, sizeof(RECT));
	//	DrawText(hdc, (TCHAR*)&valueText_, -1, &valueRect, DT_SINGLELINE | DT_CALCRECT);
	//	FillRect(hdc, &bgRect, backgroundBrush_);
	//	x = (rect_.right - valueRect.right) >> 1;
	//	TextOut(hdc, x, y, valueText_, fmw::strlen(valueText_));
	//}

	EndPaint(hWnd_, &ps);
	ValidateRect(hWnd_, NULL);
	return 0;
}

//LRESULT InputCtrl::onEraseBkgnd(HDC hDC) {
//	return 0;
//}
