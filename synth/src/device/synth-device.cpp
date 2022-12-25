#include "soundlib/src/soundplayer.h"
#include "base/memory.h"
#include "synth-device.h"
#include "player/src/player.h"

using namespace SYNTH;

#pragma region Creation
SynthDevice::SynthDevice(SynthAdapter* adapter) : ModuleDevice(NULL, (Adapter*)adapter) {
	type(SynthDevices::DeviceSynth);
	object_ = NEW_(Synth, &adapter->samplingRate, 1);
}

void SynthDevice::initialize(int voiceCount) {
	object_ = NEW_(Synth, &((SynthAdapter*)adapter_)->samplingRate, voiceCount);
}

void SynthDevice::initialize(byte** pData) {
	if (pData != NULL && *pData != NULL) {
		var voiceCount = READ(*pData, byte);
		//object_ = NEW_(Synth, ((SynthAdapter*)adapter_)->samplingRate());
		synth()->setVoiceCount(voiceCount);
		var dbId = READ(*pData, byte);
		byte* sb = ((DataBlockItem*)player_->dataBlocks().getAt(dbId))->dataBlock;
		synth()->soundBank(sb);
		var prg = READ(*pData, byte);
		synth()->setProgram(prg);
	}
}

SynthDevice::~SynthDevice() {
	DEL_((Synth*)object_);
}
#pragma endregion

#pragma region Device
void SynthDevice::setRefreshRate(float fps) {
}

int SynthDevice::run(int ticks) {
	return 0;
}
bool SynthDevice::isActive() {
	return synth()->isActive;
}
void SynthDevice::isActive(bool b) {
	synth()->isActive = b;
}

void SynthDevice::processCommand(byte cmd, byte*& cursor) {
	byte ctrlId, prgId;
	byte b;
	float f;
	switch (cmd) {
	case SynthCommands::CmdSetNote:
		byte note, velocity;
		note = READ(cursor, byte);
		velocity = READ(cursor, byte);
		setNote(note, velocity);
		break;
	case CmdSetUint8:
		ctrlId = READ(cursor, byte);
		b = READ(cursor, byte);
		setControl(ctrlId, b);
		break;
	case CmdSetFloat8:
		ctrlId = READ(cursor, byte);
		b = READ(cursor, byte);
		setControl(ctrlId, b / 256.0f);
		break;
		// case psynth.SynthAdapter.Commands.SETCTRL16:
		// 	device.setControl(sequence.getUint8(cursor++), sequence.getUint16(cursor));
		// 	cursor += 2;
		// 	break;
	case CmdSetFloat:
		ctrlId = READ(cursor, byte);
		f = READ(cursor, float);
		setControl(ctrlId, f);
		break;
	case CmdSetProgram:
		prgId = READ(cursor, byte);
		setProgram(prgId);
		break;
	}
}

Synth* SynthDevice::synth() {
	return (Synth*)object_;
}

#pragma endregion

#ifdef PLAYER_EDIT_MODE
int SynthDevice::getCommandSize(byte cmd, byte* args) {
	var length = 1;
	switch (cmd) {
	case CmdSetNote:
		length += 2 * sizeof(byte);
		break;
	case CmdSetProgram:
		length += sizeof(byte);
		break;
	default:
		length = ModuleDevice::getCommandSize(cmd, args);
		break;
	}
	return length;
}

void SynthDevice::makeCommandImpl(int command, byte*& stream, va_list args) {
	switch (command) {
	case CmdSetNote:
		*stream++ = (byte)va_arg(args, int);	// note
		*stream++ = (byte)va_arg(args, int);	// velocity
		break;
	case CmdSetProgram:
		*stream++ = (byte)va_arg(args, int);	// program id
		break;
	default:
		ModuleDevice::makeCommandImpl(command, stream, args);
		break;
	}
}

int SynthDevice::writeToStream(Stream* stream) {
	var start = stream->cursor();
	Device::writeToStream(stream);
	stream->writeByte(synth()->voices().length());
	// get data-block id
	var sb = synth()->soundBank();
	int ix = 0;
	player_->dataBlocks().search(&sb, ix, [](void* value, UINT32 ix, Collection* collection, void* key) {
		return (int)(((DataBlockItem*)value)->dataBlock - *(byte**)key);
	});
	stream->writeByte(ix);
	stream->writeByte(synth()->program());
	
	return (int)(stream->cursor() - start);
}
#endif

#pragma endregion

#pragma region Synth
void SynthDevice::setNote(byte note, byte velocity) {
	synth()->setNote(note, velocity);
}
void SynthDevice::setProgram(byte prgId) {
	synth()->setProgram(prgId);
}

byte* SynthDevice::soundBank() {
	return synth()->soundBank();
}
void SynthDevice::soundBank(byte* data) {
	synth()->soundBank(data);
}

//void SynthDevice::run(short* buffer, int start, int end) {
//	synth_->run(buffer, start, end);
//}
#pragma endregion

