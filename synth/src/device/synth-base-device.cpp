#include "soundlib/src/soundplayer.h"
#include "base/memory.h"
#include "player/src/player.h"
#include "synth/src/device/synth-base-device.h"

using namespace SYNTH;

#pragma region Creation
SynthBaseDevice::SynthBaseDevice(SynthAdapter* adapter, void* object) : ModuleDevice(adapter, object) {
}

void SynthBaseDevice::initialize(byte** pData) {
	if (pData != NULL && *pData != NULL) {
		var voiceCount = READ(*pData, byte);
		synthBase()->voiceCount(voiceCount);
		ModuleDevice::initialize(pData);
	}
}

SynthBaseDevice::~SynthBaseDevice() {
}
#pragma endregion

#pragma region Device
bool SynthBaseDevice::isActive() {
	return synthBase()->isActive();
}

void SynthBaseDevice::processCommand(byte cmd, byte*& cursor) {
	switch (cmd) {
	case ModuleCommands::CmdSetNote:
		byte note, velocity;
		note = READ(cursor, byte);
		velocity = READ(cursor, byte);
		setNote(note, velocity);
		break;
	default:
		ModuleDevice::processCommand(cmd, cursor);
	}
}

#pragma endregion

#pragma region Synth
void SynthBaseDevice::setNote(byte note, byte velocity) {
	synthBase()->setNote(note, velocity);
}
#pragma endregion