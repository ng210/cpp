#include "synth/ui/bass-ctrl.h"
#include "synth/ui/bass-ctrl.h"
//#include "synth/src/module/bass.h"

using namespace SYNTH_APP;

byte bassUiLayout[] = {
	// colors
	// background, foreground, frame, text
	DD(0x00285010), DD(0x0078ff30), DD(0x0050a020), DD(0x0040ff40),
	// controls
	// AM
	DB(BassCtrlId::amAdsrAmp),	DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(BassCtrlId::amAdsrAtk),	DB('a'),DB('t'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(BassCtrlId::amAdsrDec),	DB('d'),DB('e'), DB('c'), DB(0), DB(PotCtrlType::Knob),
	DB(BassCtrlId::amAdsrSus),	DB('s'),DB('u'), DB('s'), DB(0), DB(PotCtrlType::Knob),
	DB(BassCtrlId::amAdsrRel),	DB('r'),DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	// Filter
	LayoutNewColumn,
	DB(BassCtrlId::flt1Mode),	DB('m'),DB('o'),DB('d'),DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(BassCtrlId::flt1Cut),	DB('c'),DB('u'),DB('t'),DB(0), DB(PotCtrlType::Knob),
	DB(BassCtrlId::flt1Res),	DB('r'),DB('e'),DB('s'),DB(0), DB(PotCtrlType::Knob),
	DB(BassCtrlId::cmAdsrAmp),	DB('m'),DB('o'),DB('d'),DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	// Osc1
	LayoutNewColumn,
	DB(BassCtrlId::osc1Amp),	DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(BassCtrlId::osc1Fre),	DB('f'),DB('r'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(BassCtrlId::osc1Tune),	DB('t'),DB('u'), DB('n'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(BassCtrlId::osc1Psw),	DB('p'),DB('s'), DB('w'), DB(0), DB(PotCtrlType::Knob),
	DB(BassCtrlId::osc1Wave),	DB('w'),DB('a'), DB('v'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	LayoutVerticalGap,

	// PM
	DB(BassCtrlId::pmAdsrAmp),	DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(BassCtrlId::pmAdsrAtk),	DB('a'),DB('t'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(BassCtrlId::pmAdsrDec),	DB('d'),DB('e'), DB('c'), DB(0), DB(PotCtrlType::Knob),
	DB(BassCtrlId::pmAdsrSus),	DB('s'),DB('u'), DB('s'), DB(0), DB(PotCtrlType::Knob),
	DB(BassCtrlId::pmAdsrRel),	DB('r'),DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	// Filter
	LayoutNextColumn,
	DB(BassCtrlId::cmAdsrAtk),	DB('a'),DB('t'),DB('k'),DB(0), DB(PotCtrlType::Knob),
	DB(BassCtrlId::cmAdsrDec),	DB('d'),DB('e'),DB('c'),DB(0), DB(PotCtrlType::Knob),
	DB(BassCtrlId::cmAdsrSus),	DB('s'),DB('u'),DB('s'),DB(0), DB(PotCtrlType::Knob),
	DB(BassCtrlId::cmAdsrRel),	DB('r'),DB('e'),DB('l'),DB(0), DB(PotCtrlType::Knob),
	// Osc2					  
	LayoutNextColumn,
	DB(BassCtrlId::osc2Amp),	DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(BassCtrlId::osc2Fre),	DB('f'),DB('r'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(BassCtrlId::osc2Tune),	DB('t'),DB('u'), DB('n'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(BassCtrlId::osc2Psw),	DB('p'),DB('s'), DB('w'), DB(0), DB(PotCtrlType::Knob),
	DB(BassCtrlId::osc2Wave),	DB('w'),DB('a'), DB('v'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	LayoutEnd

	//// Osc2
	//// Flt
	//		bOsc1Amp, bOsc1Fre, bOsc1Note, bOsc1Tune, bOsc1Psw, bOsc1Wave,
	//	bOsc2Amp, bOsc2Fre, bOsc2Note, bOsc2Tune, bOsc2Psw, bOsc2Wave,

	//	bFlt1Cut,/*=fmAdsrDc*/ bFlt1Res, bFlt1Mod,/*=fmAdsrAmp*/ bFlt1Mode
};

HANDLE BassCtrl::hBackground_ = NULL;

BassCtrl::BassCtrl(Module* module) : ModuleCtrl(module) {
	hasLabel_ = true;
}

BassCtrl::~BassCtrl() {

}

void BassCtrl::create(Window* parent, char* name) {
	this->ModuleCtrl::create(parent, name);
	Stream stream(bassUiLayout, arraysize(bassUiLayout));
	stream.reset();
	unsigned long colors[] = { 0x00285010, 0x0078ff30, 0x0050a020, 0x0040ff40 };
	initFromStream(&stream, 100, colors);
	var ctrl1 = module_->getControl((int)BassCtrlId::osc1Tune);
	var ctrl2 = module_->getControl((int)BassCtrlId::osc2Tune);
	var ctrl3 = module_->getControl((int)BassCtrlId::flt1Mode);
	for (var i = 0; i < potCtrlCount_; i++) {
		if (potCtrls_[i]->pot() == ctrl1 || potCtrls_[i]->pot() == ctrl2) potCtrls_[i]->mouseSpeed2(12);
		else if (potCtrls_[i]->pot() == ctrl3) {
			potCtrls_[i]->mouseSpeed1(1);
			potCtrls_[i]->mouseSpeed2(1);
		}
	}
}

HANDLE BassCtrl::getBackgroundImage() {
	if (hBackground_ == NULL) {
		SYSFN(hBackground_, LoadImage(NULL, "bass-ctrl.bmp", IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT | LR_LOADFROMFILE));
	}
	return hBackground_;
}

//LRESULT BassCtrl::onPaint() {
//	PAINTSTRUCT ps;
//	var hdc = BeginPaint(hWnd_, &ps);
//	FillRect(hdc, &rect_, PotCtrl::foregroundBrush_);
//	EndPaint(hWnd_, &ps);
//	return 0;
//}