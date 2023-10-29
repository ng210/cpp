#include "synth/ui/stereo-delay-ctrl.h"

using namespace SYNTH_APP;

byte stereoDelayUiLayout[] = {
	// colors
	// background, foreground, frame, text
	DD(0x00285010), DD(0x0078ff30), DD(0x0050a020), DD(0x0040ff40),
	// controls
	DB(stdlFeedbackLeft), DB('f'), DB('b'), DB('a'), DB('c'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(stdlDelayLeft), DB('d'), DB('e'), DB('l'), DB('a'), DB('y'), DB(0), DB(PotCtrlType::Knob),
	DB(stdlMixLeft), DB('m'), DB('i'), DB('x'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	DB(stdlFeedbackRight), DB('f'), DB('b'), DB('a'), DB('c'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(stdlDelayRight), DB('d'), DB('e'), DB('l'), DB('a'), DB('y'), DB(0), DB(PotCtrlType::Knob),
	DB(stdlMixRight), DB('m'), DB('i'), DB('x'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	DB(stdlCut), DB('c'), DB('u'), DB('t'), DB(0), DB(PotCtrlType::Knob),
	DB(stdlRes), DB('r'), DB('e'), DB('s'), DB(0), DB(PotCtrlType::Knob),
	DB(stdlMode), DB('m'), DB('o'), DB('d'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	LayoutEnd
};

HANDLE StereoDelayCtrl::hBackground_ = NULL;

StereoDelayCtrl::StereoDelayCtrl(Module* module) : ModuleCtrl(module) {
}

StereoDelayCtrl::~StereoDelayCtrl() {

}

void StereoDelayCtrl::create(Window* parent, char* name) {
	this->ModuleCtrl::create(parent, name);

	Stream stream(stereoDelayUiLayout, arraysize(stereoDelayUiLayout));
	stream.reset();
	initFromStream(&stream);
	var ctrl1 = module_->getControl(stdlMode);
	for (var i = 0; i < potCtrlCount_; i++) {
		if (potCtrls_[i]->pot() == ctrl1) {
			potCtrls_[i]->mouseSpeed1(1);
			potCtrls_[i]->mouseSpeed2(1);
		}
	}
}

HANDLE StereoDelayCtrl::getBackgroundImage() {
	if (hBackground_ == NULL) {
		SYSFN(hBackground_, LoadImage(NULL, "stereo-delay-ctrl.bmp", IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT | LR_LOADFROMFILE));
	}
	return hBackground_;
}
