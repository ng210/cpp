#include "synth/ui/synth-ctrl.h"
#include "synth/src/module/synth.h"
//#include "synth/ui/resource.h"

using namespace SYNTH_UI;

#define SMF	 4		// synth module frame

byte synthUiLayout[] = {
	// AM
	DB(amAdsrAmp), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(amAdsrAtk), DB('a'),DB('t'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(amAdsrDec), DB('d'),DB('e'), DB('c'), DB(0), DB(PotCtrlType::Knob),
	DB(amAdsrSus), DB('s'),DB('u'), DB('s'), DB(0), DB(PotCtrlType::Knob),
	DB(amAdsrRel), DB('r'),DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	DB(amLfoAmp), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(amLfoFre), DB('f'),DB('r'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	// FM
	DB(fmAdsrAmp), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(fmAdsrAtk), DB('a'),DB('t'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(fmAdsrDec), DB('d'),DB('e'), DB('c'), DB(0), DB(PotCtrlType::Knob),
	DB(fmAdsrSus), DB('s'),DB('u'), DB('s'), DB(0), DB(PotCtrlType::Knob),
	DB(fmAdsrRel), DB('r'),DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	DB(fmLfoAmp), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(fmLfoFre), DB('f'),DB('r'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	// PM
	DB(pmAdsrAmp), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(pmAdsrAtk), DB('a'),DB('t'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(pmAdsrDec), DB('d'),DB('e'), DB('c'), DB(0), DB(PotCtrlType::Knob),
	DB(pmAdsrSus), DB('s'),DB('u'), DB('s'), DB(0), DB(PotCtrlType::Knob),
	DB(pmAdsrRel), DB('r'),DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	// Osc1
	DB(osc1Amp),	DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(osc1Fre),	DB('f'),DB('r'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(osc1Tune),	DB('t'),DB('u'), DB('n'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(osc1Psw),	DB('p'),DB('s'), DB('w'), DB(0), DB(PotCtrlType::Knob),
	DB(osc1Wave),	DB('w'),DB('a'), DB('v'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	// Osc2
	DB(osc2Amp),	DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(osc2Fre),	DB('f'),DB('r'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(osc2Tune),	DB('t'),DB('u'), DB('n'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(osc2Psw),	DB('p'),DB('s'), DB('w'), DB(0), DB(PotCtrlType::Knob),
	DB(osc2Wave),	DB('w'),DB('a'), DB('v'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	// Filter
	DB(flt1Mode),  DB('m'),DB('o'),DB('d'),DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(ftAdsrDc),  DB('c'),DB('u'),DB('t'),DB(0), DB(PotCtrlType::Knob),
	DB(flt1Res),   DB('r'),DB('e'),DB('s'),DB(0), DB(PotCtrlType::Knob),
	DB(ftAdsrAmp), DB('m'),DB('o'),DB('d'),DB(0), DB(PotCtrlType::Knob),
	DB(ftAdsrAtk), DB('a'),DB('t'),DB('k'),DB(0), DB(PotCtrlType::Knob),
	DB(ftAdsrDec), DB('d'),DB('e'),DB('c'),DB(0), DB(PotCtrlType::Knob),
	DB(ftAdsrSus), DB('s'),DB('u'),DB('s'),DB(0), DB(PotCtrlType::Knob),
	DB(ftAdsrRel), DB('r'),DB('e'),DB('l'),DB(0), DB(PotCtrlType::Knob),
	LayoutEnd
};

SynthCtrl::SynthCtrl(Module* module) : ModuleCtrl(module) {

}

SynthCtrl::~SynthCtrl() {

}

void SynthCtrl::create(Window* parent, char* name) {
	this->ModuleCtrl::create(parent, name);

	Stream stream(synthUiLayout, arraysize(synthUiLayout));
	stream.reset();
	initFromStream(&stream);
}

//LRESULT synthCtrl::onPaint() {
//	PAINTSTRUCT ps;
//	var hdc = BeginPaint(hWnd_, &ps);
//	FillRect(hdc, &rect_, PotCtrl::foregroundBrush_);
//	EndPaint(hWnd_, &ps);
//	return 0;
//}