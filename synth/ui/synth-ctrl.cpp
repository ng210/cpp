#include "synth/ui/synth-ctrl.h"
#include "synth/src/module/synth.h"
//#include "synth/ui/resource.h"

using namespace SYNTH_APP;

#define SMF	 4		// synth module frame

byte synthUiLayout[] = {
	// colors
	// background, foreground, frame, text
	DD(0x00a08030), DD(0x00f0e060), DD(0x00787030), DD(0x00fff8a0),
	// controls
	// AM
	DB(SynthCtrlId::amAdsrAmp), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::amAdsrAtk), DB('a'),DB('t'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::amAdsrDec), DB('d'),DB('e'), DB('c'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::amAdsrSus), DB('s'),DB('u'), DB('s'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::amAdsrRel), DB('r'),DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	LayoutNewColumn,
	DB(SynthCtrlId::amLfoAmp), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::amLfoFre), DB('f'),DB('r'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	// FM
	DB(SynthCtrlId::fmAdsrAmp), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::fmAdsrAtk), DB('a'),DB('t'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::fmAdsrDec), DB('d'),DB('e'), DB('c'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::fmAdsrSus), DB('s'),DB('u'), DB('s'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::fmAdsrRel), DB('r'),DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	DB(SynthCtrlId::fmLfoAmp), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::fmLfoFre), DB('f'),DB('r'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	// Osc1
	LayoutNewColumn,
	DB(SynthCtrlId::osc1Amp),	DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::osc1Fre),	DB('f'),DB('r'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::osc1Tune),	DB('t'),DB('u'), DB('n'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::osc1Psw),	DB('p'),DB('s'), DB('w'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::osc1Wave),	DB('w'),DB('a'), DB('v'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	LayoutVerticalGap,

	// PM
	DB(SynthCtrlId::pmAdsrAmp), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::pmAdsrAtk), DB('a'),DB('t'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::pmAdsrDec), DB('d'),DB('e'), DB('c'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::pmAdsrSus), DB('s'),DB('u'), DB('s'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::pmAdsrRel), DB('r'),DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	// Filter
	LayoutNextColumn,
	DB(SynthCtrlId::flt1Mode),  DB('m'),DB('o'),DB('d'),DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::flt1Cut),	DB('c'),DB('u'),DB('t'),DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::flt1Res),   DB('r'),DB('e'),DB('s'),DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::cmAdsrAmp), DB('m'),DB('o'),DB('d'),DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::cmAdsrAtk), DB('a'),DB('t'),DB('k'),DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::cmAdsrDec), DB('d'),DB('e'),DB('c'),DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::cmAdsrSus), DB('s'),DB('u'),DB('s'),DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::cmAdsrRel), DB('r'),DB('e'),DB('l'),DB(0), DB(PotCtrlType::Knob),

	// Osc2
	LayoutNextColumn,
	DB(SynthCtrlId::osc2Amp),	DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::osc2Fre),	DB('f'),DB('r'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::osc2Tune),	DB('t'),DB('u'), DB('n'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::osc2Psw),	DB('p'),DB('s'), DB('w'), DB(0), DB(PotCtrlType::Knob),
	DB(SynthCtrlId::osc2Wave),	DB('w'),DB('a'), DB('v'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	LayoutEnd
};

HANDLE SynthCtrl::hBackground_ = NULL;

SynthCtrl::SynthCtrl(Module* module) : ModuleCtrl(module) {
	hasLabel_ = true;
}

SynthCtrl::~SynthCtrl() {

}

void SynthCtrl::create(Window* parent, char* name) {
	this->ModuleCtrl::create(parent, name);

	Stream stream(synthUiLayout, arraysize(synthUiLayout));
	stream.reset();
	initFromStream(&stream);
	var ctrl1 = module_->getControl((int)SynthCtrlId::osc1Tune);
	var ctrl2 = module_->getControl((int)SynthCtrlId::osc2Tune);
	var ctrl3 = module_->getControl((int)SynthCtrlId::flt1Mode);
	for (var i = 0; i < potCtrlCount_; i++) {
		if (potCtrls_[i]->pot() == ctrl1 || potCtrls_[i]->pot() == ctrl2) potCtrls_[i]->mouseSpeed2(12);
		else if (potCtrls_[i]->pot() == ctrl3) {
			potCtrls_[i]->mouseSpeed1(1);
			potCtrls_[i]->mouseSpeed2(1);
		}
	}
}

HANDLE SynthCtrl::getBackgroundImage() {
	if (hBackground_ == NULL) {
		SYSFN(hBackground_, LoadImage(NULL, "synth-ctrl.bmp", IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT | LR_LOADFROMFILE));
	}
	return hBackground_;
}