#include "synth/ui/distort-ctrl.h"

using namespace SYNTH_UI;

byte distortUiLayout[] = {
	// colors
	// background, foreground, frame, text
	DD(0x00285010), DD(0x0078ff30), DD(0x0050a020), DD(0x0040ff40),
	// controls
	DB(distAmp), DB('a'), DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(distLvl), DB('l'), DB('v'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	DB(distCut), DB('c'), DB('u'), DB('t'), DB(0), DB(PotCtrlType::Knob),
	DB(distRes), DB('r'), DB('e'), DB('s'), DB(0), DB(PotCtrlType::Knob),
	DB(distMode), DB('m'), DB('o'), DB('d'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	LayoutEnd
};

HANDLE DistortCtrl::hBackground_ = NULL;

DistortCtrl::DistortCtrl(Module* module) : ModuleCtrl(module) {

}

DistortCtrl::~DistortCtrl() {

}

void DistortCtrl::create(Window* parent, char* name) {
	this->ModuleCtrl::create(parent, name);
	Stream stream(distortUiLayout, arraysize(distortUiLayout));
	stream.reset();
	initFromStream(&stream);
	var ctrl1 = module_->getControl(distMode);
	for (var i = 0; i < potCtrlCount_; i++) {
		if (potCtrls_[i]->pot() == ctrl1) {
			potCtrls_[i]->mouseSpeed1(1);
			potCtrls_[i]->mouseSpeed2(1);
		}
	}
}

HANDLE DistortCtrl::getBackgroundImage() {
	if (hBackground_ == NULL) {
		SYSFN(hBackground_, LoadImage(NULL, "distort-ctrl.bmp", IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT | LR_LOADFROMFILE));
	}
	return hBackground_;
}