#define _USE_MATH_DEFINES

#include <math.h>
#include "synth/ui/pot-ctrl.h"
#include <base/string.h>
#include "resource.h"

using namespace SYNTH_UI;

#define GAP 4

HBRUSH PotCtrl::backgroundBrush_ = NULL;
HBRUSH PotCtrl::foregroundBrush_ = NULL;
HBRUSH PotCtrl::frameBrush_ = NULL;
COLORREF PotCtrl::textColor_;
HFONT PotCtrl::font_ = NULL;
WndClass PotCtrl::wndClass_;
HBITMAP bitmapVPot1_ = NULL;
HDC backBuffer_ = NULL;

PotCtrl::PotCtrl() {
	if (PotCtrl::wndClass_.atom == 0) {
		PotCtrl::wndClass_.atom = registerClass("PotCtrl", NULL);
		setColors(0x00400000, 0x00c08060, 0x00804030, 0x00ffffff);
		PotCtrl::font_ = (HFONT)GetStockObject(SYSTEM_FONT);
	}
	memset((byte*)&labelSize_, 0, sizeof(SIZE) * 2);
	type_ = PotCtrlType::HPotmeter;
	pot_ = NULL;
	isCapturing_ = false;
	showLabel_ = true;
	showValue_ = true;

	backBuffer_ = NULL;
	bitmapVPot1_ = NULL;
}

PotCtrl::~PotCtrl() {

}

void PotCtrl::create(Window* parent, char* name) {
	Ctrl::create(PotCtrl::wndClass_, parent, name);
	var hdc = GetDC(hWnd_);
	label(name);
	setSize(100);
	// load and prepare images
	if (backBuffer_ == NULL) {
		var SYSFN(resInfo, FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_VPOT1), "Image"));
		if (resInfo) {
			SYSFN(bitmapVPot1_, (HBITMAP)LoadResource(GetModuleHandle(NULL), resInfo));
			SYSFN(backBuffer_, CreateCompatibleDC(hdc));
			if (backBuffer_ && bitmapVPot1_) {
				SYSPR(SelectObject(backBuffer_, bitmapVPot1_));
			}
		}
	}
	//var wi = 20, he = 110;
	//BITMAPINFO bmi = {
	//	sizeof(BITMAPINFOHEADER),	// DWORD biSize;
	//	wi,			// LONG  biWidth;
	//	he,			// LONG  biHeight;
	//	1,			// WORD  biPlanes;
	//	32,			// WORD  biBitCount;
	//	BI_RGB,		// DWORD biCompression;
	//	wi*he*4,	// DWORD biSizeImage;
	//	0,			// LONG  biXPelsPerMeter;
	//	0,			// LONG  biYPelsPerMeter;
	//	0,			// DWORD biClrUsed;
	//	0			// DWORD biClrImportant;
	//};

	ReleaseDC(hWnd_, hdc);
	//if (potV_ == NULL) {
	//	// load PNG from resource
	//	potV_ = LoadBitmap(hInstance_, MAKEINTRESOURCE(IDB_POT));
	//}
}

void PotCtrl::label(TCHAR* lbl) {
	strncpy(label_, 32, lbl);
	// get label rect
	if (showLabel_ || label_ == NULL) {
		RECT rect;
		memset(&rect, 0, sizeof(RECT));
		var hdc = GetDC(hWnd_);
		SelectObject(hdc, PotCtrl::font_);
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

void PotCtrl::setColors(DWORD background, DWORD foreground, DWORD frame, DWORD text) {
	SYSPR(DeleteObject(PotCtrl::backgroundBrush_));
	SYSPR(DeleteObject(PotCtrl::foregroundBrush_));
	SYSPR(DeleteObject(PotCtrl::frameBrush_));

	SYSFN(PotCtrl::backgroundBrush_, CreateSolidBrush(background));
	SYSFN(PotCtrl::foregroundBrush_, CreateSolidBrush(foreground));
	SYSFN(PotCtrl::frameBrush_, CreateSolidBrush(frame));
	PotCtrl::textColor_ = text;
}

void PotCtrl::setSize(int size) {
	HDC hdc;
	RECT rect;
	size_ = size;
	switch (type_) {
	case PotCtrlType::Knob:
		levelSize_.cx = levelSize_.cy = size >> 2;
		break;
	case PotCtrlType::HPotmeter:
		rect.top = 0;
		levelSize_.cx = size;
		hdc = GetDC(hWnd_);
		SelectObject(hdc, PotCtrl::font_);
		DrawText(hdc, "|", -1, &rect, DT_SINGLELINE | DT_CALCRECT);
		ReleaseDC(hWnd_, hdc);
		levelSize_.cy = rect.bottom;
		break;
	case PotCtrlType::VPotmeter:
		levelSize_.cx = size >> 2;
		levelSize_.cy = size;
		break;
	}

	updateClientRect();
}

void PotCtrl::updateClientRect(bool resize) {
	// label - labelSize_.cy
	// level - size
	// value - labelSize_.cy
	rect_.left = 0;
	rect_.top = 0;
	switch (type_) {
	case PotCtrlType::Knob:
		rect_.right = levelSize_.cx + 2 * GAP;
		rect_.bottom = GAP + labelSize_.cy + GAP + levelSize_.cy + GAP + labelSize_.cy + GAP;
		break;
	case PotCtrlType::HPotmeter:
		rect_.right = GAP + labelSize_.cx + GAP +levelSize_.cx + GAP;
		rect_.bottom = GAP + levelSize_.cy + GAP;
		break;
	case PotCtrlType::VPotmeter:
		rect_.right = levelSize_.cx + 2 * GAP;
		rect_.bottom = GAP + labelSize_.cy + GAP + levelSize_.cy + GAP + labelSize_.cy + GAP;
		break;
	}

	if (resize) {
		SYSPR(SetWindowPos(hWnd_, NULL, 0, 0, rect_.right, rect_.bottom, SWP_NOMOVE));
	}
}

void PotCtrl::type(PotCtrlType t) {
	type_ = t;
	setSize(size_);
}

void PotCtrl::pot(PotBase* p) {
	pot_ = p;
	// calculate angle
	var v = pot_->getNormalized();
	angle_ = (1.0f - v) * 300.0f - 60.0f;
}

LRESULT PotCtrl::onLeftDown(POINT& pos, WPARAM state) {
	isCapturing_ = true;
	SetCapture(hWnd_);
	return 0;
}

LRESULT PotCtrl::onLeftUp(POINT& pos, WPARAM state) {
	isCapturing_ = false;
	ReleaseCapture();
	return 0;
}

LRESULT PotCtrl::onRightUp(POINT& pos, WPARAM state) {
	return 0;
}

LRESULT PotCtrl::onMouseMove(POINT& pos, POINT& delta, WPARAM state) {
	if (isCapturing_ && (state & MK_LBUTTON)) {
		int count = state & MK_CONTROL ? 4 : 1;
		switch (type_) {
		case PotCtrlType::Knob:
			//if (delta.y < 0) pot_->dec(count);
			//else if (delta.y > 0) pot_->inc(count);
			//break;
		case PotCtrlType::HPotmeter:
			if (delta.x < 0) pot_->dec(count);
			else if (delta.x > 0) pot_->inc(count);
			//pot_->setFromNormalized((float)(pos.x - 1) / (rect_.right - 2));
			break;
		case PotCtrlType::VPotmeter:
			if (delta.y > 0) pot_->dec(count);
			else if (delta.y < 0) pot_->inc(count);
			//if (pos.y < levelRect_.bottom) {
			//	pot_->setFromNormalized(1.0f - (float)pos.y / levelRect_.bottom);
			//}
			break;
		}
		InvalidateRect(hWnd_, NULL, true);
	}
	return 0;
}

void PotCtrl::drawHPotmeter(HDC hdc, float value) {
	var x = GAP, y = GAP;
	// draw label
	if (showLabel_) {
		var cy = ((rect_.bottom - labelSize_.cy) >> 1);
		TextOut(hdc, x, cy, label_, fmw::strlen(label_));
		x += labelSize_.cx + GAP;
	}

	// draw level
	RECT rect = { x, GAP, x + levelSize_.cx, GAP + levelSize_.cy };
	FrameRect(hdc, &rect, PotCtrl::frameBrush_);
	rect.right = x + (int)floor(value * levelSize_.cx);
	FillRect(hdc, &rect, PotCtrl::foregroundBrush_);

	// draw value
	if (showValue_) {
		RECT valueRect;
		memset(&valueRect, 0, sizeof(RECT));
		DrawText(hdc, (TCHAR*)&valueText_, -1, &valueRect, DT_SINGLELINE | DT_CALCRECT);
		x += (levelSize_.cx - valueRect.right) >> 1;
		TextOut(hdc, x, GAP, valueText_, fmw::strlen(valueText_));
	}
}

void PotCtrl::drawVPotmeter(HDC hdc, float value) {
	var x = GAP, y = GAP;
	// draw label
	if (showLabel_) {
		var cx = (rect_.right - labelSize_.cx) >> 1;
		TextOut(hdc, cx, GAP, label_, fmw::strlen(label_));
		y += labelSize_.cy + GAP;
	}

	// draw level
	RECT rect = { x, y, x + levelSize_.cx, y + levelSize_.cy };
	FrameRect(hdc, &rect, PotCtrl::frameBrush_);
	rect.top = y + (int)floor((1.0f - value) * levelSize_.cy);
	FillRect(hdc, &rect, PotCtrl::foregroundBrush_);
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

void PotCtrl::drawKnob(HDC hdc, float value) {
	var y = GAP, x = GAP;
	// draw label
	if (showLabel_) {
		var cx = ((rect_.right - labelSize_.cx) >> 1);
		TextOut(hdc, cx, GAP, label_, fmw::strlen(label_));
		y += labelSize_.cy + GAP;
	}

	// draw level
	SelectObject(hdc, PotCtrl::foregroundBrush_);
	Ellipse(hdc, GAP, y, GAP + levelSize_.cx, y + levelSize_.cy);
	var cx = GAP + (levelSize_.cx >> 1);
	var cy = GAP + (levelSize_.cy >> 1);
	if (showLabel_) cy += GAP + labelSize_.cy;
	var f = (1.0f - value) * 300.0f + 60.0f;

	BeginPath(hdc);
	SelectObject(hdc, PotCtrl::frameBrush_);
	MoveToEx(hdc, cx, cy, (LPPOINT)NULL);
	AngleArc(hdc, cx, cy, levelSize_.cx>>1, 240.0f, f);

	EndPath(hdc);
	FillPath(hdc);

	//BLENDFUNCTION bf = {
	//AC_SRC_OVER,	// BlendOp
	//0,				// BlendFlags
	//0x00,			// SourceConstantAlpha
	//0				// AlphaFormat
	//};

	SYSPR(BitBlt(hdc, 0, 0, 10, 10, backBuffer_, 0, 0, SRCCOPY));

	//AngleArc(hdc, cx, cy, levelSize_.cx >> 1, 0.0f, f);
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

LRESULT PotCtrl::onPaint() {
	PAINTSTRUCT ps;
	var hdc = BeginPaint(hWnd_, &ps);
	// draw background
	FillRect(hdc, &rect_, PotCtrl::backgroundBrush_);
	SelectObject(hdc, PotCtrl::font_);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, textColor_);
	var value = pot_->getNormalized();
	if (showValue_) pot_->getValueAsString(valueText_, 32);
	switch (type_) {
	case PotCtrlType::HPotmeter: drawHPotmeter(hdc, value); break;
	case PotCtrlType::VPotmeter: drawVPotmeter(hdc, value); break;
	case PotCtrlType::Knob: drawKnob(hdc, value); break;
	}

	
	// get value rect
	//memset(&valueRect_, 0, sizeof(RECT));
	//DrawText(hdc_, (TCHAR*)&valueText_, -1, &valueRect_, DT_SINGLELINE | DT_CALCRECT);
	//labelRect_.bottom = height;
	//var x = (rect_.right - valueRect_.right) / 2;
	//var y = labelRect_.bottom + (rect_.bottom - labelRect_.bottom - valueRect_.bottom) / 2;
	//TextOut(hdc_, x, y, valueText_, fmw::strlen(valueText_));
	//DrawText(hdc_, (TCHAR*)&valueText_, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	EndPaint(hWnd_, &ps);
	ValidateRect(hWnd_, NULL);
	return 0;
}
