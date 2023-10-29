#include "synth/ui/sequence-ctrl.h"

using namespace SYNTH_APP;

WndClass SequenceCtrl::WndClass_;

SequenceCtrlConfig SequenceCtrl::DefaultConfig_ = {
	0x00004010,				//int backgroundColor;
	0x00106020,				//int gridColor;
	0x00208040,				//int lineColor;
	1,						//int lineWidth;
	DrawType::DrawLines		//DrawType type;
};

SequenceCtrl::SequenceCtrl() {
	if (SequenceCtrl::WndClass_.atom == 0) {
		SequenceCtrl::WndClass_.atom = Window::registerClass("SequenceCtrl", NULL, 0);		// CS_OWNDC ?
	}
	fmw::memcpy(&config_, &SequenceCtrl::DefaultConfig_, sizeof(SequenceCtrlConfig));
	backgroundBrush_ = NULL;
	gridBrush_ = NULL;
	lineBrush_ = NULL;
	frames_ = NULL;
}

SequenceCtrl::~SequenceCtrl() {

}

void SequenceCtrl::create(Window* parent, char* name) {
	LONG style = 0;
	DWORD exStyle = 0;
	Ctrl::create(SequenceCtrl::WndClass_, parent, name, style, exStyle);
}

void SequenceCtrl::update() {
	if (backgroundBrush_ != NULL) {
		DeleteObject(backgroundBrush_);
	}
	if (gridBrush_ != NULL) {
		DeleteObject(gridBrush_);
	}
	if (lineBrush_ != NULL) {
		DeleteObject(lineBrush_);
	}

	backgroundBrush_ = CreateSolidBrush(config_.backgroundColor);
	gridBrush_ = CreateSolidBrush(config_.gridColor);
	lineBrush_ = CreateSolidBrush(config_.lineColor);
}

LRESULT SequenceCtrl::onCreate() {
	return NULL;
}

LRESULT SequenceCtrl::onPaint() {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd_, &ps);

	FillRect(hdc, &rect_, backgroundBrush_);

	EndPaint(hWnd_, &ps);
	return NULL;
}

LRESULT SequenceCtrl::onLeftUpProc(Window* wnd, POINT& pos, WPARAM state) {
	var ctrl = (SequenceCtrl*)wnd;

	return NULL;
}

LRESULT SequenceCtrl::onRightUpProc(Window* wnd, POINT& pos, WPARAM state) {
	var ctrl = (SequenceCtrl*)wnd;

	return NULL;
}

void SequenceCtrl::insertCommand(int time, int type, byte* command) {
	var channelId = getChannelId_(command);
	frames_->insertCommand(time, channelId, command);
}

void SequenceCtrl::removeCommand(int time, int type, byte* command) {
	var channelId = getChannelId_(command);
	frames_->removeCommand(time, channelId);
}