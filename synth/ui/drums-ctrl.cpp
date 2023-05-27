#include "synth/ui/drums-ctrl.h"
//#include "synth/ui/resource.h"

using namespace SYNTH_UI;

byte genericDrumLayout[] = {
	// colors
	// background, foreground, frame, text
	DD(0x00404040), DD(0x00808080), DD(0x00c0c0c0), DD(0x00e0e0e0),
	// controls
	DB(gdType),		DB('t'), DB('y'), DB('p'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	// Dahr1
	DB(gdDahr1Amp), DB('a'), DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(gdDahr1Del), DB('d'), DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	DB(gdDahr1Atk), DB('a'), DB('t'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(gdDahr1Hld), DB('h'), DB('l'), DB('d'), DB(0), DB(PotCtrlType::Knob),
	DB(gdDahr1Rel), DB('r'), DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	// Dahr2
	DB(gdDahr2Amp), DB('a'), DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(gdDahr2Dc),  DB('d'), DB('c'),		   DB(0), DB(PotCtrlType::Knob),
	DB(gdDahr2Del), DB('d'), DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	DB(gdDahr2Atk), DB('a'), DB('t'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(gdDahr2Hld), DB('h'), DB('l'), DB('d'), DB(0), DB(PotCtrlType::Knob),
	DB(gdDahr2Rel), DB('r'), DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	// Dahr3
	DB(gdDahr3Amp), DB('a'), DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(gdDahr3Del), DB('d'), DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	DB(gdDahr3Atk), DB('a'), DB('t'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(gdDahr3Hld), DB('h'), DB('l'), DB('d'), DB(0), DB(PotCtrlType::Knob),
	DB(gdDahr3Rel), DB('r'), DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	// Filter
	DB(gdFlt1Mode), DB('m'), DB('o'), DB('d'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(gdFlt1Cut),	DB('c'), DB('u'), DB('t'), DB(0), DB(PotCtrlType::Knob),
	DB(gdFlt1Res),  DB('r'), DB('e'), DB('s'), DB(0), DB(PotCtrlType::Knob),
	DB(gdFlt2Mode), DB('m'), DB('o'), DB('d'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	DB(gdFlt2Cut),	DB('c'), DB('u'), DB('t'), DB(0), DB(PotCtrlType::Knob),
	DB(gdFlt2Res),  DB('r'), DB('e'), DB('s'), DB(0), DB(PotCtrlType::Knob),
	DB(gdDahr4Dc),	DB('m'), DB('o'), DB('d'), DB(0), DB(PotCtrlType::Knob),
	DB(gdDahr4Amp), DB('a'), DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(gdDahr4Del), DB('d'), DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	DB(gdDahr4Atk), DB('a'), DB('t'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(gdDahr4Hld), DB('h'), DB('l'), DB('d'), DB(0), DB(PotCtrlType::Knob),
	DB(gdDahr4Rel), DB('r'), DB('e'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	// Fre/Amp
	DB(gdFreq1),	DB('f'), DB('r'), DB('e'), DB('1'), DB(0), DB(PotCtrlType::Knob),
	DB(gdAmp1),		DB('a'), DB('m'), DB('p'), DB('1'), DB(0), DB(PotCtrlType::Knob),
	DB(gdFreq2),	DB('f'), DB('r'), DB('e'), DB('2'), DB(0), DB(PotCtrlType::Knob),
	DB(gdAmp2),		DB('a'), DB('m'), DB('p'), DB('2'), DB(0), DB(PotCtrlType::Knob),
	DB(gdFreq3),	DB('f'), DB('r'), DB('e'), DB('3'), DB(0), DB(PotCtrlType::Knob),
	DB(gdAmp3),		DB('a'), DB('m'), DB('p'), DB('3'), DB(0), DB(PotCtrlType::Knob),
	DB(gdFreq4),	DB('f'), DB('r'), DB('e'), DB('4'), DB(0), DB(PotCtrlType::Knob),
	DB(gdAmp4),		DB('a'), DB('m'), DB('p'), DB('4'), DB(0), DB(PotCtrlType::Knob),
	DB(gdFreq5),	DB('f'), DB('r'), DB('e'), DB('5'), DB(0), DB(PotCtrlType::Knob),
	DB(gdAmp5),		DB('a'), DB('m'), DB('p'), DB('5'), DB(0), DB(PotCtrlType::Knob),
	DB(gdFreq6),	DB('f'), DB('r'), DB('e'), DB('6'), DB(0), DB(PotCtrlType::Knob),
	DB(gdAmp6),		DB('a'), DB('m'), DB('p'), DB('6'), DB(0), DB(PotCtrlType::Knob),

	LayoutEnd
};

HANDLE GenericDrumCtrl::hBackground_ = NULL;

GenericDrumCtrl::GenericDrumCtrl(Module* module) : ModuleCtrl(module) {
	//module_->setProgram = { this, &GenericDrumCtrl::setProgramProc };
	hasLabel_ = true;
}

GenericDrumCtrl::~GenericDrumCtrl() {
}

void GenericDrumCtrl::create(Window* parent, char* name) {
	this->ModuleCtrl::create(parent, name);

	Stream stream(genericDrumLayout, arraysize(genericDrumLayout));
	stream.reset();
	initFromStream(&stream, 92);
	var ctrl1 = module_->getControl(gdFlt1Mode);
	var ctrl2 = module_->getControl(gdFlt2Mode);
	for (var i = 0; i < potCtrlCount_; i++) {
		if (potCtrls_[i]->pot() == ctrl1 || potCtrls_[i]->pot() == ctrl2) {
			potCtrls_[i]->mouseSpeed1(1);
			potCtrls_[i]->mouseSpeed2(1);
		}
	}
};

//LRESULT GenericDrumCtrl::onPaint() {
//	return ModuleCtrl::onPaint();
//}

HANDLE GenericDrumCtrl::getBackgroundImage() {
	if (hBackground_ == NULL) {
		SYSFN(hBackground_, LoadImage(NULL, "drum-ctrl.bmp", IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT | LR_LOADFROMFILE));
	}
	return hBackground_;
}



DrumsCtrl::DrumsCtrl(Module* module) : ModuleCtrl(module) {
	var drums = (Drums*)module_;
	drums->setSoundbank.add(this, &DrumsCtrl::soundbankSetter);
	for (var i = 0; i < DrumsCount; i++) {
		drumCtrls_[i] = NEW_(GenericDrumCtrl, &drums->drums[i]);
	}
}

DrumsCtrl::~DrumsCtrl() {
	for (var i = 0; i < DrumsCount; i++) {
		DEL_(drumCtrls_[i]);
	}
}

void DrumsCtrl::create(Window* parent, char* name) {
	char* defaultDrumLabels[] = { "BD", "SD", "CH", "OH" };
	this->ModuleCtrl::create(parent, name);
	for (var i = 0; i < 4; i++) {
		drumCtrls_[i]->create(this, defaultDrumLabels[i]);
	}

	int width = 0, height = 0;
	for (var i = 0; i < 4; i++) {
		SetWindowPos(drumCtrls_[i]->hWnd(), NULL, 0, height, 0, 0, SWP_NOSIZE);
		if (width < drumCtrls_[i]->rect().right) width = drumCtrls_[i]->rect().right;
		height += drumCtrls_[i]->rect().bottom;
	}

	SetWindowPos(hWnd_, NULL, 0, 0, width, height, SWP_NOMOVE);
}

GenericDrumCtrl** DrumsCtrl::drumCtrl() {
	return drumCtrls_;
}

LRESULT DrumsCtrl::onPaint() {
	//PAINTSTRUCT ps;
	//var hdc = BeginPaint(hWnd_, &ps);
	//FillRect(hdc, &rect_, PotCtrl::foregroundBrush_);
	//EndPaint(hWnd_, &ps);
	//return 0;
	return ModuleCtrl::onPaint();
}

int DrumsCtrl::soundbankSetter(void* obj, Soundbank* sb) {
	var ctrl = (DrumsCtrl*)obj;
	Drums::soundbankSetter(ctrl->module_, sb);
	return 1;
}