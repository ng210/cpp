#include "base/memory.h"
#include "player/src/device-ext.h"
#include "player/src/input-ext.h"
#include "player/src/player-device.h"

using namespace PLAYER;

DeviceExt::DeviceExt(Device* device) {
	inputs_ = NULL;
	device_ = device;
}

DeviceExt::~DeviceExt() {
}


void DeviceExt::initialize(Stream* stream) {
	var cursor = stream->cursor();
	device_->initialize(&cursor);
	stream->cursor(cursor);
	int prId = 0;
	if (stream != NULL && stream->length() > 0) {
		prId = stream->readByte();
	}
	device_->setPreset(prId);
}

void DeviceExt::createInputs() {

}

Input* DeviceExt::getInput(int id) {
	return NULL;
}

void DeviceExt::writeToStream(Stream* stream) {
	stream->writeByte(device_->type());
	stream->writeByte(device_->preset());
	writePresetToStream(stream);
}

void DeviceExt::writePresetToStream(Stream* stream) {
	var start = stream;
	for (var i = 0; i < device_->inputCount(); i++) {
		getInput(i)->writeValueToStream(stream);
	}
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
	var seq = NEW_(Sequence);
	seq->writeHeader();
	seq->writeDelta(0);
	seq->writeCommand(PlayerCommands::CmdEOS);
	return seq;
}

PresetBank* DeviceExt::loadPresetBank(const char* presetBankPath) {
	var pb = NEW_(PresetBank);

	return pb;
}
