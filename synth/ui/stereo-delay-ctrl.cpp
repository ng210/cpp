#include "synth/ui/stereo-delay-ctrl.h"

using namespace SYNTH_UI;

byte stereoDelayUiLayout[] = {
	// colors
	// background, foreground, frame, text
	DD(0x00285010), DD(0x0078ff30), DD(0x0050a020), DD(0x0040ff40),
	// controls
	DB(stdlFeedbackLeft), DB('f'), DB('e'), DB('e'), DB('d'), DB('b'), DB('a'), DB('c'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(stdlDelayLeft), DB('d'), DB('e'), DB('l'), DB('a'), DB('y'), DB(0), DB(PotCtrlType::Knob),
	DB(stdlMixLeft), DB('m'), DB('i'), DB('x'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	DB(stdlFeedbackRight), DB('f'), DB('e'), DB('e'), DB('d'), DB('b'), DB('a'), DB('c'), DB('k'), DB(0), DB(PotCtrlType::Knob),
	DB(stdlDelayRight), DB('d'), DB('e'), DB('l'), DB('a'), DB('y'), DB(0), DB(PotCtrlType::Knob),
	DB(stdlMixRight), DB('m'), DB('i'), DB('x'), DB(0), DB(PotCtrlType::Knob),
	LayoutHorizontalGap,
	DB(stdlCut), DB('c'), DB('u'), DB('t'), DB(0), DB(PotCtrlType::Knob),
	DB(stdlRes), DB('r'), DB('e'), DB('s'), DB(0), DB(PotCtrlType::Knob),
	DB(stdlMode), DB('m'), DB('o'), DB('d'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	LayoutEnd
};

StereoDelayCtrl::StereoDelayCtrl(Module* module) : ModuleCtrl(module) {
}

StereoDelayCtrl::~StereoDelayCtrl() {

}

void StereoDelayCtrl::create(Window* parent, char* name) {
	this->ModuleCtrl::create(parent, name);

	Stream stream(stereoDelayUiLayout, arraysize(stereoDelayUiLayout));
	stream.reset();
	initFromStream(&stream);
}
