#include "synth/ui/distort-ctrl.h"

using namespace SYNTH_UI;

byte distortUiLayout[] = {
	// AM
	DB(distAmp), DB('a'), DB('m'), DB('p'), DB(0), DB(PotCtrlType::Knob),
	DB(distLvl), DB('l'), DB('v'), DB('l'), DB(0), DB(PotCtrlType::Knob),
	DB(distCut), DB('c'), DB('u'), DB('t'), DB(0), DB(PotCtrlType::Knob),
	DB(distRes), DB('r'), DB('e'), DB('s'), DB(0), DB(PotCtrlType::Knob),
	DB(distMode), DB('m'), DB('o'), DB('d'), DB('e'), DB(0), DB(PotCtrlType::Knob),
	LayoutEnd
};

DistortCtrl::DistortCtrl(Module* module) : ModuleCtrl(module) {

}

DistortCtrl::~DistortCtrl() {

}

void DistortCtrl::create(Window* parent, char* name) {
	this->ModuleCtrl::create(parent, name);
	Stream stream(distortUiLayout, arraysize(distortUiLayout));
	stream.reset();
	initFromStream(&stream);
}
