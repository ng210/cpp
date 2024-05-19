#include "synth/src/ui/generic-drum-ctrl.h"

using namespace SYNTH;

//// colors
//// background, foreground, frame, text
//DD(0x00285010), DD(0x0078ff30), DD(0x0050a020), DD(0x0040ff40),

HANDLE GenericDrumCtrl::hBackground_ = NULL;

byte genericDrumUiDefaultLayout[] = {
	// Type
	DB(GenDrumType),		't','y','p','e',0,		DW(  16), DW( 48),	InputCtrlType::Knob, 100,
	// AM
	DB(GenDrumDahrAmp1),	'a','m','p',0,			DW(  50), DW( 48),	InputCtrlType::Knob, 100,
	DB(GenDrumDahrDelay1),	'd','e','l',0,			DW(  82), DW( 48),	InputCtrlType::Knob, 100,
	DB(GenDrumDahrAtk1),	'a','t','k',0,			DW( 114), DW( 48),	InputCtrlType::Knob, 100,
	DB(GenDrumDahrHold1),	'h','l','d',0,			DW( 146), DW( 48),	InputCtrlType::Knob, 100,
	DB(GenDrumDahrRel1),	'r','e','l',0,			DW( 178), DW( 48),	InputCtrlType::Knob, 100,
	// FM
	DB(GenDrumDahrAmp2),	'a','m','p',0,			DW( 212), DW( 48),	InputCtrlType::Knob, 100,
	DB(GenDrumDahrDelay2),	'd','e','l',0,			DW( 244), DW( 48),	InputCtrlType::Knob, 100,
	DB(GenDrumDahrAtk2),	'a','t','k',0,			DW( 276), DW( 48),	InputCtrlType::Knob, 100,
	DB(GenDrumDahrHold2),	'h','l','d',0,			DW( 308), DW( 48),	InputCtrlType::Knob, 100,
	DB(GenDrumDahrRel2),	'r','e','l',0,			DW( 340), DW( 48),	InputCtrlType::Knob, 100,
	// Click
	DB(GenDrumDahrAmp3),	'a','m','p',0,			DW( 374), DW( 48),	InputCtrlType::Knob, 100,
	DB(GenDrumDahrDelay3),	'd','e','l',0,			DW( 406), DW( 48),	InputCtrlType::Knob, 100,
	DB(GenDrumDahrAtk3),	'a','t','k',0,			DW( 438), DW( 48),	InputCtrlType::Knob, 100,
	DB(GenDrumDahrHold3),	'h','l','d',0,			DW( 470), DW( 48),	InputCtrlType::Knob, 100,
	DB(GenDrumDahrRel3),	'r','e','l',0,			DW( 502), DW( 48),	InputCtrlType::Knob, 100,
	// CM
	DB(GenDrumDahrAmp4),	'a','m','p',0,			DW( 536), DW( 48),	InputCtrlType::Knob, 100,
	DB(GenDrumDahrDelay4),	'd','e','l',0,			DW( 568), DW( 48),	InputCtrlType::Knob, 100,
	DB(GenDrumDahrAtk4),	'a','t','k',0,			DW( 600), DW( 48),	InputCtrlType::Knob, 100,
	DB(GenDrumDahrHold4),	'h','l','d',0,			DW( 632), DW( 48),	InputCtrlType::Knob, 100,
	DB(GenDrumDahrRel4),	'r','e','l',0,			DW( 664), DW( 48),	InputCtrlType::Knob, 100,
	// Flt1
	DB(GenDrumFlt1Mode),	'm','o','d','e',0,		DW( 698), DW( 48),	InputCtrlType::Knob, 100,
	DB(GenDrumFlt1Cut),		'c','u','t',0,			DW( 730), DW( 48),	InputCtrlType::Knob, 100,
	DB(GenDrumFlt1Res),		'r','e','s',0,			DW( 762), DW( 48),	InputCtrlType::Knob, 100,
	// Flt2											    
	DB(GenDrumFlt2Mode),	'm','o','d','e',0,		DW( 796), DW( 48),	InputCtrlType::Knob, 100,
	DB(GenDrumFlt2Cut),		'c','u','t',0,			DW( 828), DW( 48),	InputCtrlType::Knob, 100,
	DB(GenDrumFlt2Res),		'r','e','s',0,			DW( 860), DW( 48),	InputCtrlType::Knob, 100,
	// Osc1
	DB(GenDrumAmp1),		'a','m','p',0,			DW( 894), DW( 49), 	InputCtrlType::Knob, 100,
	DB(GenDrumFre1),		'f','r','e',0,			DW( 926), DW( 49), 	InputCtrlType::Knob, 100,
	// Osc2
	DB(GenDrumAmp2),		'a','m','p',0,			DW( 960), DW( 49), 	InputCtrlType::Knob, 100,
	DB(GenDrumFre2),		'f','r','e',0,			DW( 992), DW( 49), 	InputCtrlType::Knob, 100,
	// Osc3
	DB(GenDrumAmp3),		'a','m','p',0,			DW(1026), DW( 49), 	InputCtrlType::Knob, 100,
	DB(GenDrumFre3),		'f','r','e',0,			DW(1058), DW( 49), 	InputCtrlType::Knob, 100,
	// Osc4
	DB(GenDrumAmp4),		'a','m','p',0,			DW(1092), DW( 49), 	InputCtrlType::Knob, 100,
	DB(GenDrumFre4),		'f','r','e',0,			DW(1124), DW( 49), 	InputCtrlType::Knob, 100,
	// Osc5
	DB(GenDrumAmp5),		'a','m','p',0,			DW(1158), DW( 49), 	InputCtrlType::Knob, 100,
	DB(GenDrumFre5),		'f','r','e',0,			DW(1190), DW( 49), 	InputCtrlType::Knob, 100,
	// Osc6
	DB(GenDrumAmp6),		'a','m','p',0,			DW(1224), DW( 49), 	InputCtrlType::Knob, 100,
	DB(GenDrumFre6),		'f','r','e',0,			DW(1256), DW( 49), 	InputCtrlType::Knob, 100
};

GenericDrumCtrl::GenericDrumCtrl(DeviceExt* deviceExt) : ModuleCtrl(deviceExt) {
	// background, foreground, frame, text
	colors[0] = 0x00202020;
	colors[1] = 0x00a0a0a0;
	colors[2] = 0x00606060;
	colors[3] = 0x00e0e0e0;
	borderWidth_ = 0;
	inputSpacing_ = 1;
}

GenericDrumCtrl::~GenericDrumCtrl() {

}

void GenericDrumCtrl::create(Window* parent, char* name) {
	Stream stream(genericDrumUiDefaultLayout, arraysize(genericDrumUiDefaultLayout));
	toolbarRect_.left = 16 + borderWidth_;
	toolbarRect_.top = 4 + borderWidth_;
	presetCtrl_.setSize(8 * 16, 8 * 24);
	DeviceCtrl::create(parent, name, &stream);
	setSize(1290, 108);
	//inputControls_[BassInputOsc1Tune].mouseSpeed2(12);
	//inputControls_[BassInputOsc2Tune].mouseSpeed2(12);
	//var ctrl = &inputControls_[BassInputFlt1Mode];
	//ctrl->mouseSpeed1(1);
	//ctrl->mouseSpeed2(1);
	//ctrl->mouseSpeed3(1);
}

void GenericDrumCtrl::loadBackgroundImage() {
	if (GenericDrumCtrl::hBackground_ == NULL) {
		SYSFN(GenericDrumCtrl::hBackground_, LoadImage(NULL, "generic-drum-ctrl.bmp", IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT | LR_LOADFROMFILE));
	}
	hBackgroundImage_ = GenericDrumCtrl::hBackground_;
}
