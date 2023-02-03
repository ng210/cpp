#include "soundlib/src/soundplayer.h"
#include "base/memory.h"
#include "player/src/player.h"
#include "synth/src/device/synth-base-device.h"
#include "synth/src/module/synth.h"

using namespace SYNTH;

#pragma region Creation
SynthBaseDevice::SynthBaseDevice(SynthAdapter* adapter, void* object) : ModuleDevice(NULL, (Adapter*)adapter) {

}

void SynthBaseDevice::initialize(byte** pData) {
	if (pData != NULL && *pData != NULL) {
		var voiceCount = READ(*pData, byte);
		//object_ = NEW_(Synth, ((SynthAdapter*)adapter_)->samplingRate());
		synthBase()->voiceCount(voiceCount);
		var dbId = READ(*pData, byte);
		byte* sb = ((DataBlockItem*)player_->dataBlocks().get(dbId))->dataBlock;
		synthBase()->soundBank(sb);
		var prg = READ(*pData, byte);
		synthBase()->setProgram(prg);
	}
}

SynthBaseDevice::~SynthBaseDevice() {
}
#pragma endregion

#pragma region Device
void SynthBaseDevice::setRefreshRate(float fps) {
}

int SynthBaseDevice::run(int ticks) {
	return 0;
}
bool SynthBaseDevice::isActive() {
	return synthBase()->isActive();
}
//void SynthBaseDevice::isActive(bool b) {
//	synth()->isActive = b;
//}

void SynthBaseDevice::processCommand(byte cmd, byte*& cursor) {
	byte ctrlId, prgId;
	byte b;
	float f;
	switch (cmd) {
	case ModuleCommands::CmdSetNote:
		byte note, velocity;
		note = READ(cursor, byte);
		velocity = READ(cursor, byte);
		setNote(note, velocity);
		break;
	case ModuleCommands::CmdSetUint8:
		ctrlId = READ(cursor, byte);
		b = READ(cursor, byte);
		setControl(ctrlId, b);
		break;
	case ModuleCommands::CmdSetFloat8:
		ctrlId = READ(cursor, byte);
		b = READ(cursor, byte);
		setControl(ctrlId, b / 255.0f);
		break;
		// case psynth.SynthAdapter.Commands.SETCTRL16:
		// 	device.setControl(sequence.getUint8(cursor++), sequence.getUint16(cursor));
		// 	cursor += 2;
		// 	break;
	case ModuleCommands::CmdSetFloat:
		ctrlId = READ(cursor, byte);
		f = READ(cursor, float);
		setControl(ctrlId, f);
		break;
	case ModuleCommands::CmdSetProgram:
		prgId = READ(cursor, byte);
		setProgram(prgId);
		break;
	}
}

#pragma endregion

#ifdef PLAYER_EDIT_MODE

int SynthBaseDevice::writeToStream(Stream* stream) {
	var start = stream->cursor();
	Device::writeToStream(stream);
	stream->writeByte(synthBase()->voiceCount());
	// get data-block id
	var sb = synthBase()->soundBank();
	int ix = 0;
	player_->dataBlocks().search(&sb, ix, [](void* value, Key key, UINT32 ix, Collection* collection, void* args) {
		return (int)(((DataBlockItem*)value)->dataBlock - (byte*)key.p);
		});
	stream->writeByte(ix);
	stream->writeByte(synthBase()->program());

	return (int)(stream->cursor() - start);
}
#endif

#pragma endregion

#pragma region Synth
void SynthBaseDevice::setNote(byte note, byte velocity) {
	synthBase()->setNote(note, velocity);
}
void SynthBaseDevice::setProgram(byte prgId) {
	synthBase()->setProgram(prgId);
}

byte* SynthBaseDevice::soundBank() {
	return synthBase()->soundBank();
}
void SynthBaseDevice::soundBank(byte* data) {
	synthBase()->soundBank(data);
}

//void SynthBaseDevice::run(short* buffer, int start, int end) {
//	synth_->run(buffer, start, end);
//}
#pragma endregion

