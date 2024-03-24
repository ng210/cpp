#include "soundlib/src/soundplayer.h"
#include "base/memory.h"
#include "player/src/player.h"
#include "synth/src/device/synth-base-device.h"

using namespace SYNTH;

#pragma region Creation
SynthBaseDevice::SynthBaseDevice(SynthAdapter* adapter, Player* player, void* object) : ModuleDevice(adapter, player, object) {
}

void SynthBaseDevice::initialize(byte** pData) {
	if (pData != NULL && *pData != NULL) {
		ModuleDevice::initialize(pData);
		var voiceCount = READ(*pData, byte);
		synthBase()->voiceCount(voiceCount);
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

void SynthBaseDevice::setNote(byte note, byte velocity) {
	synthBase()->setNote(note, velocity);
}
