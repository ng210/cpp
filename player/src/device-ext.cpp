#include "base/memory.h"
#include "player/src/device-ext.h"
#include "player/src/player-device.h"

using namespace PLAYER;

DeviceExt::DeviceExt(void* object, Adapter* adapter) : Device(object, adapter) {
	inputs_ = NULL;
	inputCount_ = 0;
}

void DeviceExt::initializeFromStream(byte** pData) {
	int prId = 0;
	if (pData != NULL && *pData != NULL) {
		prId = READ(*pData, byte);
	}
	setPreset(prId);
}

int DeviceExt::writeToStream(Stream* stream) {
	stream->writeByte(type_);
	stream->writeByte(preset_);
	return 1;
}

int DeviceExt::writePreset(byte* pData) {
	var start = pData;
	for (var i = 0; i < inputCount_; i++) {
		inputs_[i].writeToStream(pData);
	}
	return (int)(pData - start);
}

Stream* DeviceExt::makeCommand(byte command, ...) {
	va_list args;
	va_start(args, command);
	var stream = NEW_(Stream, getCommandSize(&command));
	stream->writeByte(command);
	makeCommandImpl(command, stream, args);
	va_end(args);
	return stream;
}

Sequence* DeviceExt::createDefaultSequence() {
	var seq = NEW_(Sequence, this);
	seq->writeHeader();
	seq->writeDelta(0);
	seq->writeCommand(PlayerCommands::CmdEOS);
	return seq;
}
