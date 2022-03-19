#include <windows.h>
#include "base/memory.h"
#include "synth-adapter.h"
#include "player/src/player.h"
#include "soundlib/src/soundplayer.h"
#include "synth-device.h"

NS_FW_BASE_USE
using namespace SYNTH;
using namespace PLAYER;

AdapterInfo SynthAdapter::Info = { "SynthAdapter", 0, SynthAdapter::creator, SynthAdapter::initializer };

AdapterInfo* SynthAdapter::info() { return &SynthAdapter::Info; };
SynthAdapter::SynthAdapter(Player* player) {
	player_ = player;
	frame_ = 0;
	this->samplingRate_ = 48000;
	setRefreshRate(120.0 / 60);
}
SynthAdapter::~SynthAdapter() {

}
void SynthAdapter::prepareContext(byte* data) {
	samplingRate_ = READ(data, word);

	// default bpm = 80
	samplePerFrame_ = samplingRate_ * 3.75f / 80;
	Adapter::prepareContext(data);

	//SoundPlayer::start(samplingRate_, 2, SynthAdapter::fillSoundBuffer, this);

}
Device* SynthAdapter::createDeviceImpl(int deviceType, byte** initData) {
	byte*& data = *initData;
	SynthDevice* device = NULL;
	switch (deviceType) {
	case DevSynth:
		var voiceCount = READ(data, byte);
		if (voiceCount != 0) {
			device = NEW_(SynthDevice, samplingRate_, voiceCount);
			var udbId = READ(data, byte);
			byte* sb; sb = ((UserDataBlockItem*)player()->userDataBlocks().getAt(udbId))->userDataBlock;
			device->soundBank(sb);
			device->setProgram(0);
		}
		break;
	//case psynth.SynthAdapter.Device.DELAY:
	//	// todo: add presets
	//	device = new psynth.Delay(sound.sampleRate);
	//	break;
	}
	return device;
}
byte* SynthAdapter::processCommand(Channel* channel, byte command) {
	var device = (SynthDevice*)channel->device();
	var sequence = channel->sequence();
	var cursor = channel->cursor();

	byte ctrlId, prgId;
	byte b;
	float f;
	switch (command) {
	case CmdSetNote:
		byte note, velocity;
		note = READ(cursor, byte);
		velocity = READ(cursor, byte);
		device->setNote(note, velocity);
		break;
	case CmdSetUint8:
		ctrlId = READ(cursor, byte);
		b = READ(cursor, byte);
		device->setControl(ctrlId, b);
		break;
	case CmdSetFloat8:
		ctrlId = READ(cursor, byte);
		b = READ(cursor, byte);
		device->setControl(ctrlId, b / 256.0f);
		break;
		// case psynth.SynthAdapter.Commands.SETCTRL16:
		// 	device.setControl(sequence.getUint8(cursor++), sequence.getUint16(cursor));
		// 	cursor += 2;
		// 	break;
	case CmdSetFloat:
		ctrlId = READ(cursor, byte);
		f = READ(cursor, float);
		device->setControl(ctrlId, f);
		break;
	case CmdSetProgram:
		prgId = READ(cursor, byte);
		device->setProgram(prgId);
		break;
	}
	channel->cursor(cursor);
	return cursor;
}
void SynthAdapter::setRefreshRate(float fps) {
	samplePerFrame_ = samplingRate_ / fps;
}

byte* SynthAdapter::makeCommand(byte command, Sequence* sequence, byte* cursor) {
	return NULL;
}

void SynthAdapter::fillSoundBuffer(short* buffer, int bufferSize, void* args) {
	var start = 0;
	var end = 0;
	var ad = (SynthAdapter*)args;
	var player = ad->player();
	var remains = bufferSize;
	for (var i = 0; i < bufferSize; i++) {
		buffer[i] = 0;
	}
	var frame = ad->frame();
	while (remains) {
		//if (frame == 0) {
		//	if (!player.run(1)) {
		//		player.reset();
		//		player.run(0)
		//	}
		//	ad->frame() += ad->samplePerFrame();
		//}
		var len = frame < remains ? frame : remains;
		end = start + len;
		frame -= len;
		for (var i = 0; i < ad->devices().length(); i++) {
			var dev = (SynthDevice*)ad->getDevice(i);
			dev->run(buffer, start, end);
		}

		start = end;
		remains -= len;
	}
	ad->frame(frame);
}

Adapter* SynthAdapter::creator(Player* player) {
	var synthAdapter = NEW_(SynthAdapter, player);
	return synthAdapter;
}
void SynthAdapter::initializer(void* args) {

}
