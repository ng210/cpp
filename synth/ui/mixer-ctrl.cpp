#include "synth/ui/mixer-ctrl.h"

using namespace SYNTH_UI;

MixerCtrl::MixerCtrl(Module* module) : ModuleCtrl(module) {
}

MixerCtrl::~MixerCtrl() {

}

Stream* MixerCtrl::createVerticalLayout() {
	var stream = NEW_(Stream, 1024);
	// colors
	// background, foreground, frame, text
	stream->writeDword(0x00285010);
	stream->writeDword(0x0078ff30);
	stream->writeDword(0x0050a020);
	stream->writeDword(0x0040ff40);
	var mixer = (Mixer8x4*)module_;
	int offset = 0;
	for (var i = 0; i < mixer->channelCount_; i++) {
		stream->writeByte(MxCh1amp + offset)->writeString("amp")->writeByte(PotCtrlType::Knob);
		stream->writeByte(MxCh1pan + offset)->writeString("pan")->writeByte(PotCtrlType::Knob);
		stream->writeByte(MxCh1gain + offset)->writeString("gain")->writeByte(PotCtrlType::Knob)->writeByte(LayoutHorizontalGap);
		offset += MixerChCtrlCount;
	}
	stream->writeByte(LayoutVerticalGap);

	for (var j = 0; j < 4; j++) {
		offset = MxCh1st1gain + j;
		for (var i = 0; i < mixer->channelCount_; i++) {
			stream->writeByte(offset)->writeString("gain")->writeByte(PotCtrlType::Knob)->writeByte(LayoutHorizontalGap);
			offset += MixerChCtrlCount;
		}
		stream->writeByte(LayoutVerticalGap);
	}

	stream->writeByte(LayoutEnd);
	return stream;
}
Stream* MixerCtrl::createHorizontalLayout() {
	var stream = NEW_(Stream, 1024);
	// colors
	// background, foreground, frame, text
	stream->writeDword(0x00285010);
	stream->writeDword(0x0078ff30);
	stream->writeDword(0x0050a020);
	stream->writeDword(0x0040ff40);
	// controls
	var mixer = (Mixer8x4*)module_;
	int offset = MxCh1amp;
	for (var i = 0; i < mixer->channelCount(); i++) {
		stream->writeByte(offset + MxCh1amp)->writeString("amp")->writeByte(PotCtrlType::Knob);
		stream->writeByte(offset + MxCh1pan)->writeString("pan")->writeByte(PotCtrlType::Knob);
		stream->writeByte(offset + MxCh1gain)->writeString("gain")->writeByte(PotCtrlType::Knob);
		stream->writeByte(LayoutHorizontalGap);
		var ch = mixer->getChannel(i);
		for (var j = 0; j < ch->stageCount; j++) {
			stream->writeByte(offset + MxCh1st1gain + j)->writeString("gain")->writeByte(PotCtrlType::Knob);
		}
		offset += MixerChCtrlCount;
		stream->writeByte(LayoutVerticalGap);
	}
	stream->writeByte(LayoutEnd);
	return stream;
}

void MixerCtrl::updateLayout() {
	// enable/disable channels and stages
	var map = createChildWindowMap();
	var mixer = (Mixer8x4*)module_;
	int offset = 0;
	for (var i = 0; i < 8; i++) {
		var hWnd = (HWND)map->values()->get(offset + i);
		EnableWindow(hWnd, false);
		offset += MixerChCtrlCount;
	}
	DEL_(map);
}


void MixerCtrl::create(Window* parent, char* name) {
	this->ModuleCtrl::create(parent, name);
	//var layout = createVerticalLayout();
	var layout = createHorizontalLayout();
	layout->reset();
	initFromStream(layout);
	DEL_(layout);
	updateLayout();
}
