#include "base/memory.h"
#include "player/src/device-ext.h"
#include "player/src/input-ext.h"
#include "player/src/player-device.h"

using namespace PLAYER;

DeviceExt::DeviceExt(Device* device) {
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

void DeviceExt::setupInputs() {}

void DeviceExt::writeToStream(Stream* stream) {
	stream->writeByte(device_->type());
	//stream->writeByte(device_->preset());
	//writePresetsToStream(stream);
}

void DeviceExt::writePresetToStream(Stream * stream) {
	for (var i = 0; i < device_->inputCount(); i++) {
		device_->getInput(i)->writeValueToStream(stream);
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

void DeviceExt::presetAdder(void* obj, Stream* preset, void* args) {
	var that = (DeviceExt*)obj;
	var dev = that->device_;
	var name = preset->readString();
	var prg = preset->cursor();
	dev->presetBank()->add(name, prg);
	var ix = dev->presetBank()->indexOf(name);
	dev->setPreset(ix);
}

void DeviceExt::presetRemover(void* obj, int ix, void* args) {
	var that = (DeviceExt*)obj;
	var dev = that->device_;
	var pb = dev->presetBank();
	var name = pb->keys()->get(ix);
	if (name != NULL) {
		pb->remove(name);
		if (pb->length() > 0) {
			if (ix == pb->length() - 1) ix--;
			dev->setPreset(ix);
		}
	}	
}

PresetBank* DeviceExt::loadPresetBank(const char* presetBankPath) {
	var pb = NEW_(PresetBank);

	return pb;
}
