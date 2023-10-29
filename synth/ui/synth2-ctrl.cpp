#include "synth/ui/synth2-ctrl.h"
#include "synth/src/module/synth.h"
//#include "synth/ui/resource.h"

using namespace SYNTH_APP;

#define SMF	 4		// synth module frame

byte synth2UiLayout[] = {
	// colors
	// background, foreground, frame, text
	DD(0x0040c0d0), DD(0x0040c0d0), DD(0x0050e0ff), DD(0x00206068),
	// controls
	// Osc1
	DB(Synth2CtrlId::osc1Amp),	DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::osc1Fre),	DB('f'),DB('r'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::osc1Tune),	DB('t'),DB('u'), DB('n'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::osc1Psw),	DB('p'),DB('s'), DB('w'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::osc1Wave),	DB('w'),DB('a'), DB('v'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	// AM
	DB(Synth2CtrlId::amAdsrAmp), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::amAdsrAtk), DB('a'),DB('t'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::amAdsrDec), DB('d'),DB('e'), DB('c'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::amAdsrSus), DB('s'),DB('u'), DB('s'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::amAdsrRel), DB('r'),DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	DB(Synth2CtrlId::amLfoAmp), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::amLfoFre), DB('f'),DB('r'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	DB(Synth2CtrlId::amOsc1),	DB('o'),DB('s'), DB('c'), DB('1'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::amOsc2),	DB('o'),DB('s'), DB('c'), DB('2'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	// FM
	LayoutNewColumn,
	DB(Synth2CtrlId::fmAdsrAmp), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::fmAdsrAtk), DB('a'),DB('t'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::fmAdsrDec), DB('d'),DB('e'), DB('c'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::fmAdsrSus), DB('s'),DB('u'), DB('s'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::fmAdsrRel), DB('r'),DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	DB(Synth2CtrlId::fmLfoAmp), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::fmLfoFre), DB('f'),DB('r'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	DB(Synth2CtrlId::fmOsc1),	DB('o'),DB('s'), DB('c'), DB('1'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::fmOsc2),	DB('o'),DB('s'), DB('c'), DB('2'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	// Flt env
	DB(Synth2CtrlId::cmAdsrAmp), DB('a'),DB('m'),DB('p'),DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::cmAdsrAtk), DB('a'),DB('t'),DB('k'),DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::cmAdsrDec), DB('d'),DB('e'),DB('c'),DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::cmAdsrSus), DB('s'),DB('u'),DB('s'),DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::cmAdsrRel), DB('r'),DB('e'),DB('l'),DB(0), DB(PotCtrlType::Knob),
	LayoutVerticalGap,

	// Osc2
	DB(Synth2CtrlId::osc2Amp),	DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::osc2Fre),	DB('f'),DB('r'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::osc2Tune),	DB('t'),DB('u'), DB('n'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::osc2Psw),	DB('p'),DB('s'), DB('w'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::osc2Wave),	DB('w'),DB('a'), DB('v'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	// PM
	DB(Synth2CtrlId::pmAdsrAmp), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::pmAdsrAtk), DB('a'),DB('t'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::pmAdsrDec), DB('d'),DB('e'), DB('c'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::pmAdsrSus), DB('s'),DB('u'), DB('s'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::pmAdsrRel), DB('r'),DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	DB(Synth2CtrlId::pmLfoAmp), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::pmLfoFre), DB('f'),DB('r'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	DB(Synth2CtrlId::pmOsc1),	DB('o'),DB('s'), DB('c'), DB('1'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::pmOsc2),	DB('o'),DB('s'), DB('c'), DB('2'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	// Flt1
	DB(Synth2CtrlId::flt1Amp),	DB('m'),DB('i'),DB('x'),DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::flt11Cut),	DB('c'),DB('u'),DB('t'),DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::flt1Band),	DB('b'),DB('a'),DB('n'),DB('d'),DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::flt11Res),	DB('r'),DB('e'),DB('s'),DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	DB(Synth2CtrlId::flt2Amp),	DB('m'),DB('i'),DB('x'),DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::flt21Cut),	DB('c'),DB('u'),DB('t'),DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::flt2Band),	DB('b'),DB('a'),DB('n'),DB('d'),DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::flt21Res),	DB('r'),DB('e'),DB('s'),DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	DB(Synth2CtrlId::flt3Amp),	DB('m'),DB('i'),DB('x'),DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::flt31Cut),	DB('c'),DB('u'),DB('t'),DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::flt3Band),	DB('b'),DB('a'),DB('n'),DB('d'),DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::flt31Res),	DB('r'),DB('e'),DB('s'),DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	DB(Synth2CtrlId::cmLfoAmp), DB('a'),DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(Synth2CtrlId::cmLfoFre), DB('f'),DB('r'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	LayoutEnd
};

HANDLE Synth2Ctrl::hBackground_ = NULL;

Synth2Ctrl::Synth2Ctrl(Module* module) : ModuleCtrl(module) {
	hasLabel_ = true;
}

Synth2Ctrl::~Synth2Ctrl() {

}

void Synth2Ctrl::create(Window* parent, char* name) {
	this->ModuleCtrl::create(parent, name);

	Stream stream(synth2UiLayout, arraysize(synth2UiLayout));
	stream.reset();
	initFromStream(&stream);
	module_->getControl((int)Synth2CtrlId::fmOsc1)->max.f = 128.0f;
	module_->getControl((int)Synth2CtrlId::fmOsc2)->max.f = 128.0f;
	//var ctrl1 = module_->getControl((int)Synth2CtrlId::osc1Tune);
	//var ctrl2 = module_->getControl((int)Synth2CtrlId::osc2Tune);
	//var ctrl3 = module_->getControl((int)Synth2CtrlId::flt11Mode);
	//for (var i = 0; i < potCtrlCount_; i++) {
	//	if (potCtrls_[i]->pot() == ctrl1 || potCtrls_[i]->pot() == ctrl2) potCtrls_[i]->mouseSpeed2(12);
	//	else if (potCtrls_[i]->pot() == ctrl3) {
	//		potCtrls_[i]->mouseSpeed1(1);
	//		potCtrls_[i]->mouseSpeed2(1);
	//	}
	//}
}

HANDLE Synth2Ctrl::getBackgroundImage() {
	if (hBackground_ == NULL) {
		SYSFN(hBackground_, LoadImage(NULL, "synth2-ctrl.bmp", IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT | LR_LOADFROMFILE));
	}
	return hBackground_;
}