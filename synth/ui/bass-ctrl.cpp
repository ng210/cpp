#include "synth/ui/bass-ctrl.h"
#include "synth/src/module/bass.h"
//#include "synth/ui/resource.h"

using namespace SYNTH_UI;

#define GLH 16		// group label height
#define BMF	 4		// bass module frame

byte initData[] = {
	// count
	DB(28),
	// AM
	DB(bAmAdsrAmp), DW((BMF + 0)), DW((BMF + 1 * GLH + 0 * 80)), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(bAmAdsrAtk), DW((BMF + 42)), DW((BMF + 1 * GLH + 0 * 80)), DB('a'),DB('t'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(bAmAdsrDec), DW((BMF + 84)), DW((BMF + 1 * GLH + 0 * 80)), DB('d'),DB('e'), DB('c'), DB(0), DB(PotCtrlType::Knob),
	DB(bAmAdsrSus), DW((BMF + 126)), DW((BMF + 1 * GLH + 0 * 80)), DB('s'),DB('u'), DB('s'), DB(0), DB(PotCtrlType::Knob),
	DB(bAmAdsrRel), DW((BMF + 168)), DW((BMF + 1 * GLH + 0 * 80)), DB('r'),DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	// PM
	DB(bPmAdsrAmp), DW((BMF + 0)), DW((BMF + 2 * GLH + 1 * 80)), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(bPmAdsrAtk), DW((BMF + 42)), DW((BMF + 2 * GLH + 1 * 80)), DB('a'),DB('t'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(bPmAdsrDec), DW((BMF + 84)), DW((BMF + 2 * GLH + 1 * 80)), DB('d'),DB('e'), DB('c'), DB(0), DB(PotCtrlType::Knob),
	DB(bPmAdsrSus), DW((BMF + 126)), DW((BMF + 2 * GLH + 1 * 80)), DB('s'),DB('u'), DB('s'), DB(0), DB(PotCtrlType::Knob),
	DB(bPmAdsrRel), DW((BMF + 168)), DW((BMF + 2 * GLH + 1 * 80)), DB('r'),DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	// Filter
	DB(bFlt1Mode),  DW((BMF + 0)), DW((BMF + 3 * GLH + 2 * 80)), DB('m'),DB('o'),DB('d'),DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(bFtAdsrDc),  DW((BMF + 42)), DW((BMF + 3 * GLH + 2 * 80)), DB('c'),DB('u'),DB('t'),DB(0), DB(PotCtrlType::Knob),
	DB(bFlt1Res),   DW((BMF + 84)), DW((BMF + 3 * GLH + 2 * 80)), DB('r'),DB('e'),DB('s'),DB(0), DB(PotCtrlType::Knob),
	DB(bFtAdsrAmp), DW((BMF + 126)), DW((BMF + 3 * GLH + 2 * 80)), DB('m'),DB('o'),DB('d'),DB(0), DB(PotCtrlType::Knob),
	DB(bFtAdsrAtk), DW((BMF + 168)), DW((BMF + 3 * GLH + 2 * 80)), DB('a'),DB('t'),DB('k'),DB(0), DB(PotCtrlType::Knob),
	DB(bFtAdsrDec), DW((BMF + 210)), DW((BMF + 3 * GLH + 2 * 80)), DB('d'),DB('e'),DB('c'),DB(0), DB(PotCtrlType::Knob),
	DB(bFtAdsrSus), DW((BMF + 252)), DW((BMF + 3 * GLH + 2 * 80)), DB('s'),DB('u'),DB('s'),DB(0), DB(PotCtrlType::Knob),
	DB(bFtAdsrRel), DW((BMF + 294)), DW((BMF + 3 * GLH + 2 * 80)), DB('r'),DB('e'),DB('l'),DB(0), DB(PotCtrlType::Knob),
	// Osc1
	DB(bOsc1Amp), DW((2*BMF + 208 + 0)), DW((BMF + 1 * GLH + 0 * 80)), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(bOsc1Fre), DW((2*BMF + 208 + 42)), DW((BMF + 1 * GLH + 0 * 80)), DB('f'),DB('r'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(bOsc1Tune),DW((2*BMF + 208 + 84)), DW((BMF + 1 * GLH + 0 * 80)), DB('t'),DB('u'), DB('n'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(bOsc1Psw), DW((2*BMF + 208 + 126)), DW((BMF + 1 * GLH + 0 * 80)), DB('p'),DB('s'), DB('w'), DB(0), DB(PotCtrlType::Knob),
	DB(bOsc1Wave),DW((2*BMF + 208 + 168)), DW((BMF + 1 * GLH + 0 * 80)), DB('w'),DB('a'), DB('v'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	// Osc2					  
	DB(bOsc2Amp), DW((2*BMF + 208 + 0)), DW((BMF + 2 * GLH + 1 * 80)), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(bOsc2Fre), DW((2*BMF + 208 + 42)), DW((BMF + 2 * GLH + 1 * 80)), DB('f'),DB('r'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(bOsc2Tune),DW((2*BMF + 208 + 84)), DW((BMF + 2 * GLH + 1 * 80)), DB('t'),DB('u'), DB('n'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(bOsc2Psw), DW((2*BMF + 208 + 126)), DW((BMF + 2 * GLH + 1 * 80)), DB('p'),DB('s'), DB('w'), DB(0), DB(PotCtrlType::Knob),
	DB(bOsc2Wave),DW((2*BMF + 208 + 168)), DW((BMF + 2 * GLH + 1 * 80)), DB('w'),DB('a'), DB('v'), DB('e'), DB(0), DB(PotCtrlType::Knob)


	//// Osc2
	//// Flt
	//		bOsc1Amp, bOsc1Fre, bOsc1Note, bOsc1Tune, bOsc1Psw, bOsc1Wave,
	//	bOsc2Amp, bOsc2Fre, bOsc2Note, bOsc2Tune, bOsc2Psw, bOsc2Wave,

	//	bFlt1Cut,/*=fmAdsrDc*/ bFlt1Res, bFlt1Mod,/*=fmAdsrAmp*/ bFlt1Mode
};
HBITMAP potImg = NULL;

BassCtrl::BassCtrl(Module* module) : ModuleCtrl(module) {
	
}

BassCtrl::~BassCtrl() {

}

void BassCtrl::create(Window* parent, char* name) {
	this->ModuleCtrl::create(parent, name);

	Stream stream(initData, arraysize(initData));
	stream.reset();
	initFromStream(&stream);
	rect_ = { 0, 0, 3 * BMF + 2 * 208, 2 * BMF + 3 * GLH + 3 * 80 };
	SetWindowPos(hWnd_, NULL, 0, 0, rect_.right, rect_.bottom, SWP_NOMOVE | SWP_SHOWWINDOW);

	//if (!potImg) {
	//	SYSFN(potImg, LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_POT1)));
	//}
}

//LRESULT BassCtrl::onPaint() {
//	PAINTSTRUCT ps;
//	var hdc = BeginPaint(hWnd_, &ps);
//	FillRect(hdc, &rect_, PotCtrl::foregroundBrush_);
//	EndPaint(hWnd_, &ps);
//	return 0;
//}