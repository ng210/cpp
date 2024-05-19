#include "synth/src/ui/bass-ctrl.h"

using namespace SYNTH;

//// colors
//// background, foreground, frame, text
//DD(0x00285010), DD(0x0078ff30), DD(0x0050a020), DD(0x0040ff40),

HANDLE BassCtrl::hBackground_ = NULL;

byte bassUiLayout[] = {
	// AM
	DB(BassInputAmAmp),		DB('a'),DB('m'),DB('p'),0,			DW( 22), DW( 49),	InputCtrlType::Knob, 100,
	DB(BassInputAmAtk),		DB('a'),DB('t'),DB('k'),0,			DW( 52), DW( 49),	InputCtrlType::Knob, 100,
	DB(BassInputAmDec),		DB('d'),DB('e'),DB('c'),0,			DW( 82), DW( 49),	InputCtrlType::Knob, 100,
	DB(BassInputAmSus),		DB('s'),DB('u'),DB('s'),0,			DW(112), DW( 49),	InputCtrlType::Knob, 100,
	DB(BassInputAmRel),		DB('r'),DB('e'),DB('l'),0,			DW(142), DW( 49),	InputCtrlType::Knob, 100,
	// PM
	DB(BassInputPmAmp),		DB('a'),DB('m'),DB('p'),0,			DW( 22), DW(132),	InputCtrlType::Knob, 100,
	DB(BassInputPmAtk),		DB('a'),DB('t'),DB('k'),0,			DW( 52), DW(132),	InputCtrlType::Knob, 100,
	DB(BassInputPmDec),		DB('d'),DB('e'),DB('c'),0,			DW( 82), DW(132),	InputCtrlType::Knob, 100,
	DB(BassInputPmSus),		DB('s'),DB('u'),DB('s'),0,			DW(112), DW(132),	InputCtrlType::Knob, 100,
	DB(BassInputPmRel),		DB('r'),DB('e'),DB('l'),0,			DW(142), DW(132),	InputCtrlType::Knob, 100,
	// CM
	DB(BassInputFtAtk),		DB('a'),DB('t'),DB('k'),0,			DW(176), DW(132),	InputCtrlType::Knob, 100,
	DB(BassInputFtDec),		DB('d'),DB('e'),DB('c'),0,			DW(206), DW(132),	InputCtrlType::Knob, 100,
	DB(BassInputFtSus),		DB('s'),DB('u'),DB('s'),0,			DW(236), DW(132),	InputCtrlType::Knob, 100,
	DB(BassInputFtRel),		DB('r'),DB('e'),DB('l'),0,			DW(266), DW(132),	InputCtrlType::Knob, 100,
	// Flt1
	DB(BassInputFlt1Mode),	DB('m'),DB('o'),DB('d'),DB('e'),0,	DW(176), DW( 49),	InputCtrlType::Knob, 100,
	DB(BassInputFlt1Cut),	DB('c'),DB('u'),DB('t'),0,			DW(206), DW( 49),	InputCtrlType::Knob, 100,
	DB(BassInputFtAmp),		DB('m'),DB('o'),DB('d'),0,			DW(236), DW( 49),	InputCtrlType::Knob, 100,
	DB(BassInputFlt1Res),	DB('r'),DB('e'),DB('s'),0,			DW(266), DW( 49),	InputCtrlType::Knob, 100,
	// Osc1
	DB(BassInputOsc1Amp),	DB('a'),DB('m'),DB('p'),0,			DW(300), DW( 49), 	InputCtrlType::Knob, 100,
	DB(BassInputOsc1Fre),	DB('f'),DB('r'),DB('e'),0,			DW(330), DW( 49), 	InputCtrlType::Knob, 100,
	DB(BassInputOsc1Tune),	DB('t'),DB('u'),DB('n'),DB('e'),0,	DW(360), DW( 49), 	InputCtrlType::Knob, 100,
	DB(BassInputOsc1Psw),	DB('p'),DB('s'),DB('w'),0,			DW(390), DW( 49), 	InputCtrlType::Knob, 100,
	DB(BassInputOsc1Wave),	DB('w'),DB('a'),DB('v'),DB('e'),0,	DW(420), DW( 49), 	InputCtrlType::Knob, 100,
	// Osc2
	DB(BassInputOsc2Amp),	DB('a'),DB('m'),DB('p'),0,			DW(300), DW(132), 	InputCtrlType::Knob, 100,
	DB(BassInputOsc2Fre),	DB('f'),DB('r'),DB('e'),0,			DW(330), DW(132), 	InputCtrlType::Knob, 100,
	DB(BassInputOsc2Tune),	DB('t'),DB('u'),DB('n'),DB('e'),0,	DW(360), DW(132), 	InputCtrlType::Knob, 100,
	DB(BassInputOsc2Psw),	DB('p'),DB('s'),DB('w'),0,			DW(390), DW(132), 	InputCtrlType::Knob, 100,
	DB(BassInputOsc2Wave),	DB('w'),DB('a'),DB('v'),DB('e'),0,	DW(420), DW(132), 	InputCtrlType::Knob, 100
};

byte bassUiLayout2[] = {
	// AM
	DB(BassInputAmAmp),		DB('a'),DB('m'),DB('p'),0,			DW( 16), DW( 48),	InputCtrlType::Knob, 100,
	DB(BassInputAmAtk),		DB('a'),DB('t'),DB('k'),0,			DW( 48), DW( 48),	InputCtrlType::Knob, 100,
	DB(BassInputAmDec),		DB('d'),DB('e'),DB('c'),0,			DW( 80), DW( 48),	InputCtrlType::Knob, 100,
	DB(BassInputAmSus),		DB('s'),DB('u'),DB('s'),0,			DW(112), DW( 48),	InputCtrlType::Knob, 100,
	DB(BassInputAmRel),		DB('r'),DB('e'),DB('l'),0,			DW(144), DW( 48),	InputCtrlType::Knob, 100,
	// PM
	DB(BassInputPmAmp),		DB('a'),DB('m'),DB('p'),0,			DW(178), DW( 48),	InputCtrlType::Knob, 100,
	DB(BassInputPmAtk),		DB('a'),DB('t'),DB('k'),0,			DW(210), DW( 48),	InputCtrlType::Knob, 100,
	DB(BassInputPmDec),		DB('d'),DB('e'),DB('c'),0,			DW(242), DW( 48),	InputCtrlType::Knob, 100,
	DB(BassInputPmSus),		DB('s'),DB('u'),DB('s'),0,			DW(274), DW( 48),	InputCtrlType::Knob, 100,
	DB(BassInputPmRel),		DB('r'),DB('e'),DB('l'),0,			DW(306), DW( 48),	InputCtrlType::Knob, 100,
	// Flt1
	DB(BassInputFlt1Mode),	DB('m'),DB('o'),DB('d'),DB('e'),0,	DW(340), DW( 48),	InputCtrlType::Knob, 100,
	DB(BassInputFlt1Cut),	DB('c'),DB('u'),DB('t'),0,			DW(372), DW( 48),	InputCtrlType::Knob, 100,
	DB(BassInputFtAmp),		DB('m'),DB('o'),DB('d'),0,			DW(404), DW( 48),	InputCtrlType::Knob, 100,
	DB(BassInputFlt1Res),	DB('r'),DB('e'),DB('s'),0,			DW(436), DW( 48),	InputCtrlType::Knob, 100,
	DB(BassInputFtAtk),		DB('a'),DB('t'),DB('k'),0,			DW(468), DW( 48),	InputCtrlType::Knob, 100,
	DB(BassInputFtDec),		DB('d'),DB('e'),DB('c'),0,			DW(500), DW( 48),	InputCtrlType::Knob, 100,
	DB(BassInputFtSus),		DB('s'),DB('u'),DB('s'),0,			DW(532), DW( 48),	InputCtrlType::Knob, 100,
	DB(BassInputFtRel),		DB('r'),DB('e'),DB('l'),0,			DW(564), DW( 48),	InputCtrlType::Knob, 100,
	// Osc1
	DB(BassInputOsc1Amp),	DB('a'),DB('m'),DB('p'),0,			DW(598), DW( 48), 	InputCtrlType::Knob, 100,
	DB(BassInputOsc1Fre),	DB('f'),DB('r'),DB('e'),0,			DW(630), DW( 48), 	InputCtrlType::Knob, 100,
	DB(BassInputOsc1Tune),	DB('t'),DB('u'),DB('n'),DB('e'),0,	DW(662), DW( 48), 	InputCtrlType::Knob, 100,
	DB(BassInputOsc1Psw),	DB('p'),DB('s'),DB('w'),0,			DW(694), DW( 48), 	InputCtrlType::Knob, 100,
	DB(BassInputOsc1Wave),	DB('w'),DB('a'),DB('v'),DB('e'),0,	DW(726), DW( 48), 	InputCtrlType::Knob, 100,
	// Osc2
	DB(BassInputOsc2Amp),	DB('a'),DB('m'),DB('p'),0,			DW(760), DW( 48), 	InputCtrlType::Knob, 100,
	DB(BassInputOsc2Fre),	DB('f'),DB('r'),DB('e'),0,			DW(792), DW( 48), 	InputCtrlType::Knob, 100,
	DB(BassInputOsc2Tune),	DB('t'),DB('u'),DB('n'),DB('e'),0,	DW(824), DW( 48), 	InputCtrlType::Knob, 100,
	DB(BassInputOsc2Psw),	DB('p'),DB('s'),DB('w'),0,			DW(856), DW( 48), 	InputCtrlType::Knob, 100,
	DB(BassInputOsc2Wave),	DB('w'),DB('a'),DB('v'),DB('e'),0,	DW(888), DW( 48), 	InputCtrlType::Knob, 100
};

BassCtrl::BassCtrl(DeviceExt* deviceExt) : ModuleCtrl(deviceExt) {
	// background, foreground, frame, text
	colors[0] = 0x00285010;
	colors[1] = 0x0078e030;
	colors[2] = 0x00208010;
	colors[3] = 0x0040ff40;
	borderWidth_ = 0;
	inputSpacing_ = 1;
}

BassCtrl::~BassCtrl() {

}

void BassCtrl::create(Window* parent, char* name) {
	Stream stream(bassUiLayout2, arraysize(bassUiLayout2));
	toolbarRect_.left = 16 + borderWidth_;
	toolbarRect_.top = 4 + borderWidth_;
	presetCtrl_.setSize(8 * 16, 8 * 24);
	DeviceCtrl::create(parent, name, &stream);
	//var bassDev = (BassDevice*)deviceExt_->device();
	//unsigned long colors[] = { 0x00285010, 0x0078ff30, 0x0050a020, 0x0040ff40 };
	//setSize(452, 198);
	setSize(888 + 32 + 4, 48+60);
	//background_.setSize(rect_.right, rect_.bottom);
	inputControls_[BassInputOsc1Tune].mouseSpeed2(12);
	inputControls_[BassInputOsc2Tune].mouseSpeed2(12);
	var ctrl = &inputControls_[BassInputFlt1Mode];
	ctrl->mouseSpeed1(1);
	ctrl->mouseSpeed2(1);
	ctrl->mouseSpeed3(1);
}

void BassCtrl::loadBackgroundImage() {
	if (BassCtrl::hBackground_ == NULL) {
		SYSFN(BassCtrl::hBackground_, LoadImage(NULL, "bass-ctrl.bmp", IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT | LR_LOADFROMFILE));
	}
	hBackgroundImage_ = BassCtrl::hBackground_;
}


//LRESULT BassCtrl::onPaint() {
//	PAINTSTRUCT ps;
//	var hdc = BeginPaint(hWnd_, &ps);
//	FillRect(hdc, &rect_, PotCtrl::foregroundBrush_);
//	EndPaint(hWnd_, &ps);
//	return 0;
//}