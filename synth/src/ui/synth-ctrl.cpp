#include "synth/src/ui/synth-ctrl.h"

using namespace SYNTH;

HANDLE SynthCtrl::hBackground_ = NULL;

byte synthUiLayout[] = {
	// AM
	DB(SynthInputAmEnvAmp),		DB('a'),DB('m'),DB('p'),0,			DW( 22), DW( 49),	InputCtrlType::Knob, 100,
	DB(SynthInputAmEnvAtk),		DB('a'),DB('t'),DB('k'),0,			DW( 52), DW( 49),	InputCtrlType::Knob, 100,
	DB(SynthInputAmEnvDec),		DB('d'),DB('e'),DB('c'),0,			DW( 82), DW( 49),	InputCtrlType::Knob, 100,
	DB(SynthInputAmEnvSus),		DB('s'),DB('u'),DB('s'),0,			DW(112), DW( 49),	InputCtrlType::Knob, 100,
	DB(SynthInputAmEnvRel),		DB('r'),DB('e'),DB('l'),0,			DW(142), DW( 49),	InputCtrlType::Knob, 100,

	// FM
	DB(SynthInputFmEnvAmp),		DB('a'),DB('m'),DB('p'),0,			DW(240), DW( 49),	InputCtrlType::Knob, 100,
	DB(SynthInputFmEnvAtk),		DB('a'),DB('t'),DB('k'),0,			DW(270), DW( 49),	InputCtrlType::Knob, 100,
	DB(SynthInputFmEnvDec),		DB('d'),DB('e'),DB('c'),0,			DW(300), DW( 49),	InputCtrlType::Knob, 100,
	DB(SynthInputFmEnvSus),		DB('s'),DB('u'),DB('s'),0,			DW(330), DW( 49),	InputCtrlType::Knob, 100,
	DB(SynthInputFmEnvRel),		DB('r'),DB('e'),DB('l'),0,			DW(360), DW( 49),	InputCtrlType::Knob, 100,

	// PM
	DB(SynthInputPmEnvAmp),		DB('a'),DB('m'), DB('p'),0,			DW( 22), DW(132),	InputCtrlType::Knob, 100,
	DB(SynthInputPmEnvAtk),		DB('a'),DB('t'), DB('k'),0,			DW( 52), DW(132),	InputCtrlType::Knob, 100,
	DB(SynthInputPmEnvDec),		DB('d'),DB('e'), DB('c'),0,			DW( 82), DW(132),	InputCtrlType::Knob, 100,
	DB(SynthInputPmEnvSus),		DB('s'),DB('u'), DB('s'),0,			DW(112), DW(132),	InputCtrlType::Knob, 100,
	DB(SynthInputPmEnvRel),		DB('r'),DB('e'), DB('l'),0,			DW(142), DW(132),	InputCtrlType::Knob, 100,

	// Filter
	DB(SynthInputFlt1Mode),		DB('m'),DB('o'),DB('d'),DB('e'),0,	DW(176), DW(132),	InputCtrlType::Knob, 100,
	DB(SynthInputFlt1Cut),		DB('c'),DB('u'),DB('t'),0,			DW(206), DW(132),	InputCtrlType::Knob, 100,
	DB(SynthInputFlt1Res),		DB('r'),DB('e'),DB('s'),0,			DW(236), DW(132),	InputCtrlType::Knob, 100,
	DB(SynthInputFtEnvAmp),		DB('m'),DB('o'),DB('d'),0,			DW(266), DW(132),	InputCtrlType::Knob, 100,
	DB(SynthInputFtEnvAtk),		DB('a'),DB('t'),DB('k'),0,			DW(296), DW(132),	InputCtrlType::Knob, 100,
	DB(SynthInputFtEnvDec),		DB('d'),DB('e'),DB('c'),0,			DW(326), DW(132),	InputCtrlType::Knob, 100,
	DB(SynthInputFtEnvSus),		DB('s'),DB('u'),DB('s'),0,			DW(356), DW(132),	InputCtrlType::Knob, 100,
	DB(SynthInputFtEnvRel),		DB('r'),DB('e'),DB('l'),0,			DW(386), DW(132),	InputCtrlType::Knob, 100,

	DB(SynthInputAmLfoAmp),		DB('a'),DB('m'),DB('p'),0,			DW(176), DW( 49),	InputCtrlType::Knob, 100,
	DB(SynthInputAmLfoFre),		DB('f'),DB('r'),DB('e'),0,			DW(206), DW( 49),	InputCtrlType::Knob, 100,

	DB(SynthInputFmLfoAmp),		DB('a'),DB('m'),DB('p'),0,			DW(394), DW( 49),	InputCtrlType::Knob, 100,
	DB(SynthInputFmLfoFre),		DB('f'),DB('r'),DB('e'),0,			DW(424), DW( 49),	InputCtrlType::Knob, 100,

	// Osc1
	DB(SynthInputOsc1Amp),		DB('a'),DB('m'),DB('p'),0,			DW(458), DW( 49),	InputCtrlType::Knob, 100,
	DB(SynthInputOsc1Fre),		DB('f'),DB('r'),DB('e'),0,			DW(488), DW( 49),	InputCtrlType::Knob, 100,
	DB(SynthInputOsc1Tune),		DB('t'),DB('u'),DB('n'),DB('e'),0,	DW(518), DW( 49),	InputCtrlType::Knob, 100,
	DB(SynthInputOsc1Psw),		DB('p'),DB('s'),DB('w'),0,			DW(548), DW( 49),	InputCtrlType::Knob, 100,
	DB(SynthInputOsc1Wave),		DB('w'),DB('a'),DB('v'),DB('e'),0,	DW(578), DW( 49),	InputCtrlType::Knob, 100,

	// Osc2
	DB(SynthInputOsc2Amp),		DB('a'),DB('m'),DB('p'),0,			DW(458), DW(132),	InputCtrlType::Knob, 100,
	DB(SynthInputOsc2Fre),		DB('f'),DB('r'),DB('e'),0,			DW(488), DW(132),	InputCtrlType::Knob, 100,
	DB(SynthInputOsc2Tune),		DB('t'),DB('u'),DB('n'),DB('e'),0,	DW(518), DW(132),	InputCtrlType::Knob, 100,
	DB(SynthInputOsc2Psw),		DB('p'),DB('s'),DB('w'),0,			DW(548), DW(132),	InputCtrlType::Knob, 100,
	DB(SynthInputOsc2Wave),		DB('w'),DB('a'),DB('v'),DB('e'),0,	DW(578), DW(132),	InputCtrlType::Knob, 100
};

byte synthUiLayout2[] = {
	// AM
	DB(SynthInputAmEnvAmp),		DB('a'),DB('m'),DB('p'),0,			DW(  16), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputAmEnvAtk),		DB('a'),DB('t'),DB('k'),0,			DW(  48), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputAmEnvDec),		DB('d'),DB('e'),DB('c'),0,			DW(  80), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputAmEnvSus),		DB('s'),DB('u'),DB('s'),0,			DW( 112), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputAmEnvRel),		DB('r'),DB('e'),DB('l'),0,			DW( 144), DW(48),	InputCtrlType::Knob, 100,
	// LFO-A
	DB(SynthInputAmLfoAmp),		DB('a'),DB('m'),DB('p'),0,			DW( 178), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputAmLfoFre),		DB('f'),DB('r'),DB('e'),0,			DW( 210), DW(48),	InputCtrlType::Knob, 100,
	// FM
	DB(SynthInputFmEnvAmp),		DB('a'),DB('m'),DB('p'),0,			DW( 244), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputFmEnvAtk),		DB('a'),DB('t'),DB('k'),0,			DW( 276), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputFmEnvDec),		DB('d'),DB('e'),DB('c'),0,			DW( 308), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputFmEnvSus),		DB('s'),DB('u'),DB('s'),0,			DW( 340), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputFmEnvRel),		DB('r'),DB('e'),DB('l'),0,			DW( 372), DW(48),	InputCtrlType::Knob, 100,
	// LFO-F
	DB(SynthInputFmLfoAmp),		DB('a'),DB('m'),DB('p'),0,			DW( 406), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputFmLfoFre),		DB('f'),DB('r'),DB('e'),0,			DW( 438), DW(48),	InputCtrlType::Knob, 100,
	// PM
	DB(SynthInputPmEnvAmp),		DB('a'),DB('m'), DB('p'),0,			DW( 472), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputPmEnvAtk),		DB('a'),DB('t'), DB('k'),0,			DW( 504), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputPmEnvDec),		DB('d'),DB('e'), DB('c'),0,			DW( 536), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputPmEnvSus),		DB('s'),DB('u'), DB('s'),0,			DW( 568), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputPmEnvRel),		DB('r'),DB('e'), DB('l'),0,			DW( 600), DW(48),	InputCtrlType::Knob, 100,
	// Filter
	DB(SynthInputFlt1Mode),		DB('m'),DB('o'),DB('d'),DB('e'),0,	DW( 634), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputFlt1Cut),		DB('c'),DB('u'),DB('t'),0,			DW( 666), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputFtEnvAmp),		DB('m'),DB('o'),DB('d'),0,			DW( 698), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputFlt1Res),		DB('r'),DB('e'),DB('s'),0,			DW( 730), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputFtEnvAtk),		DB('a'),DB('t'),DB('k'),0,			DW( 762), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputFtEnvDec),		DB('d'),DB('e'),DB('c'),0,			DW( 794), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputFtEnvSus),		DB('s'),DB('u'),DB('s'),0,			DW( 826), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputFtEnvRel),		DB('r'),DB('e'),DB('l'),0,			DW( 858), DW(48),	InputCtrlType::Knob, 100,
	// Osc1
	DB(SynthInputOsc1Amp),		DB('a'),DB('m'),DB('p'),0,			DW( 892), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputOsc1Fre),		DB('f'),DB('r'),DB('e'),0,			DW( 924), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputOsc1Tune),		DB('t'),DB('u'),DB('n'),DB('e'),0,	DW( 956), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputOsc1Psw),		DB('p'),DB('s'),DB('w'),0,			DW( 988), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputOsc1Wave),		DB('w'),DB('a'),DB('v'),DB('e'),0,	DW(1020), DW(48),	InputCtrlType::Knob, 100,
	// Osc2
	DB(SynthInputOsc2Amp),		DB('a'),DB('m'),DB('p'),0,			DW(1054), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputOsc2Fre),		DB('f'),DB('r'),DB('e'),0,			DW(1086), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputOsc2Tune),		DB('t'),DB('u'),DB('n'),DB('e'),0,	DW(1118), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputOsc2Psw),		DB('p'),DB('s'),DB('w'),0,			DW(1150), DW(48),	InputCtrlType::Knob, 100,
	DB(SynthInputOsc2Wave),		DB('w'),DB('a'),DB('v'),DB('e'),0,	DW(1182), DW(48),	InputCtrlType::Knob, 100
};

SynthCtrl::SynthCtrl(DeviceExt* deviceExt) : ModuleCtrl(deviceExt) {
	// background, foreground, frame, text
	colors[0] = 0x00a08030;
	colors[1] = 0x00e0b848;
	colors[2] = 0x00787030;
	colors[3] = 0x00fff8a0;
	borderWidth_ = 0;
}

SynthCtrl::~SynthCtrl() {

}

void SynthCtrl::create(Window* parent, char* name) {
	Stream stream(synthUiLayout2, arraysize(synthUiLayout2));
	toolbarRect_.left = 22 + borderWidth_;
	toolbarRect_.top = 4 + borderWidth_;
	presetCtrl_.setSize(8 * 16, 8 * 24);
	DeviceCtrl::create(parent, name, &stream);
	setSize(1216, 48+60);
}

void SynthCtrl::loadBackgroundImage() {
	if (SynthCtrl::hBackground_ == NULL) {
		SYSFN(SynthCtrl::hBackground_, LoadImage(NULL, "synth-ctrl.bmp", IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT | LR_LOADFROMFILE));
	}
	hBackgroundImage_ = SynthCtrl::hBackground_;
}