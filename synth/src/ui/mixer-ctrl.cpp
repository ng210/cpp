#include "synth/src/ui/mixer-ctrl.h"

HANDLE MixerCtrl::hBackground_ = NULL;

MixerCtrl::MixerCtrl(DeviceExt* deviceExt) : ModuleCtrl(deviceExt) {
	// background, foreground, frame, text
	colors[0] = 0x00202020;
	colors[1] = 0x00c0c0c0;
	colors[2] = 0x00404040;
	colors[3] = 0x00e0e0e0;
	borderWidth_ = 2;
}

MixerCtrl::~MixerCtrl() {

}

void MixerCtrl::create(Window* parent, char* name) {
	Stream stream(1024);
	int id = 0;
	int x = 15 + borderWidth_, y;
	char lbl[16];
	for (var ci = 1; ci < 9; ci++) {
		y = 19 + borderWidth_;
		//input id, label, x, y, type
		stream.writeByte(id++)->writeString("amp")->writeWord(x)->writeWord(y + 40)->writeByte(InputCtrlType::VPotmeter /*| InputCtrlNoValue*/)->writeByte(180);
		stream.writeByte(id++)->writeString("pan")->writeWord(x + 4)->writeWord(y)->writeByte(InputCtrlType::Knob | InputCtrlNoValue)->writeByte(100);
		x += 42;
		stream.writeByte(id++)->writeString("gain")->writeWord(x)->writeWord(y)->writeByte(InputCtrlType::Knob | InputCtrlNoValue)->writeByte(100);
		y += 40;
		for (var si = 1; si < 5; si++) {
			sprintf_s(lbl, "st%d", si);
			stream.writeByte(id++)->writeString(lbl)->writeWord(x)->writeWord(y)->writeByte(InputCtrlType::Knob | InputCtrlNoValue)->writeByte(80);
			y += 32;
		}
		x += 32;
	}
	//Stream stream(mixerUiLayoutH, arraysize(mixerUiLayoutH));
	//toolbarRect_.left = 22 + borderWidth_;
	//toolbarRect_.top = 4 + borderWidth_;
	//presetCtrl_.setSize(8 * 16, 8 * 24);
	stream.rewind();
	DeviceCtrl::create(parent, name, &stream);
	setSize(610, 192);
}


void MixerCtrl::loadBackgroundImage() {
	if (MixerCtrl::hBackground_ == NULL) {
		SYSFN(MixerCtrl::hBackground_, LoadImage(NULL, "mixer-ctrl.bmp", IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT | LR_LOADFROMFILE));
	}
	hBackgroundImage_ = MixerCtrl::hBackground_;
}