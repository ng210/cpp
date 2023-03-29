#include "synth/ui/bass-ctrl.h"
//#include "synth/src/module/bass.h"

using namespace SYNTH_UI;

byte bassUiLayout[] = {
	// AM
	DB(bAmAdsrAmp), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(bAmAdsrAtk), DB('a'),DB('t'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(bAmAdsrDec), DB('d'),DB('e'), DB('c'), DB(0), DB(PotCtrlType::Knob),
	DB(bAmAdsrSus), DB('s'),DB('u'), DB('s'), DB(0), DB(PotCtrlType::Knob),
	DB(bAmAdsrRel), DB('r'),DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	// PM
	DB(bPmAdsrAmp), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(bPmAdsrAtk), DB('a'),DB('t'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(bPmAdsrDec), DB('d'),DB('e'), DB('c'), DB(0), DB(PotCtrlType::Knob),
	DB(bPmAdsrSus), DB('s'),DB('u'), DB('s'), DB(0), DB(PotCtrlType::Knob),
	DB(bPmAdsrRel), DB('r'),DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	// Filter
	DB(bFlt1Mode),  DB('m'),DB('o'),DB('d'),DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(bFtAdsrDc),  DB('c'),DB('u'),DB('t'),DB(0), DB(PotCtrlType::Knob),
	DB(bFlt1Res),   DB('r'),DB('e'),DB('s'),DB(0), DB(PotCtrlType::Knob),
	DB(bFtAdsrAmp), DB('m'),DB('o'),DB('d'),DB(0), DB(PotCtrlType::Knob),
	DB(bFtAdsrAtk), DB('a'),DB('t'),DB('k'),DB(0), DB(PotCtrlType::Knob),
	DB(bFtAdsrDec), DB('d'),DB('e'),DB('c'),DB(0), DB(PotCtrlType::Knob),
	DB(bFtAdsrSus), DB('s'),DB('u'),DB('s'),DB(0), DB(PotCtrlType::Knob),
	DB(bFtAdsrRel), DB('r'),DB('e'),DB('l'),DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	// Osc1
	DB(bOsc1Amp),	DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(bOsc1Fre),	DB('f'),DB('r'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(bOsc1Tune),	DB('t'),DB('u'), DB('n'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(bOsc1Psw),	DB('p'),DB('s'), DB('w'), DB(0), DB(PotCtrlType::Knob),
	DB(bOsc1Wave),	DB('w'),DB('a'), DB('v'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	// Osc2					  
	DB(bOsc2Amp),	DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(bOsc2Fre),	DB('f'),DB('r'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(bOsc2Tune),	DB('t'),DB('u'), DB('n'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(bOsc2Psw),	DB('p'),DB('s'), DB('w'), DB(0), DB(PotCtrlType::Knob),
	DB(bOsc2Wave),	DB('w'),DB('a'), DB('v'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	LayoutEnd

	//// Osc2
	//// Flt
	//		bOsc1Amp, bOsc1Fre, bOsc1Note, bOsc1Tune, bOsc1Psw, bOsc1Wave,
	//	bOsc2Amp, bOsc2Fre, bOsc2Note, bOsc2Tune, bOsc2Psw, bOsc2Wave,

	//	bFlt1Cut,/*=fmAdsrDc*/ bFlt1Res, bFlt1Mod,/*=fmAdsrAmp*/ bFlt1Mode
};

BassCtrl::BassCtrl(Module* module) : ModuleCtrl(module) {
	
}

BassCtrl::~BassCtrl() {

}

void BassCtrl::create(Window* parent, char* name) {
	this->ModuleCtrl::create(parent, name);

	Stream stream(bassUiLayout, arraysize(bassUiLayout));
	stream.reset();
	initFromStream(&stream);
}

//LRESULT BassCtrl::onPaint() {
//	PAINTSTRUCT ps;
//	var hdc = BeginPaint(hWnd_, &ps);
//	FillRect(hdc, &rect_, PotCtrl::foregroundBrush_);
//	EndPaint(hWnd_, &ps);
//	return 0;
//}