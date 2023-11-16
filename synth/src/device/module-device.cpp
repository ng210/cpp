#include "base/memory.h"
#include "player/src/player.h"
#include "synth/src/device/synth-adapter.h"
#include "synth/src/device/module-device.h"
#include "synth/src/module/module.h"

using namespace SYNTH;

ModuleDevice::ModuleDevice(void* object, Adapter* adapter) : Device(object, adapter) {
}

void ModuleDevice::initialize(byte** pData) {
	// set soundbank
	Soundbank* sb = module()->getDefaultSoundbank();
	int prgId = 0;
	if (pData != NULL && *pData != NULL) {
		datablockId_ = READ(*pData, byte);
		var db = ((DataBlockItem*)player_->dataBlocks().get(datablockId_));
		if (db != NULL) {
			var sb = module()->createSoundbank();
			sb->data(db->dataBlock);
			module()->setSoundbank(sb);
		}
		module()->setSoundbank(sb);
	}
	// set program and further initializations
	((Module*)object_)->initializeFromStream(pData);
}

bool ModuleDevice::isActive() {
	return ((Module*)object_)->isActive();
}

int ModuleDevice::run(int ticks) {
	return 0;
}
void ModuleDevice::setRefreshRate(float fps) {

}
void ModuleDevice::processCommand(byte cmd, byte*& cursor) {
	byte ctrlId, b;
	float f;
	switch (cmd) {
	case ModuleCommands::CmdSetUint8:
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
	}
}

PotBase* ModuleDevice::getControl(byte ctrlId) {
	return ((Module*)object_)->getControl(ctrlId);
}
void ModuleDevice::setControl(byte ctrlId, S value) {
	((Module*)object_)->setControl(ctrlId, value);
}
void ModuleDevice::setSoundbank(Soundbank* soundbank) {
	var mdl = (Module*)object_;
	mdl->setSoundbank(soundbank);
}

Soundbank* ModuleDevice::soundbank() {
	return ((Module*)object_)->soundbank();
}

void ModuleDevice::setProgram(int prgId) {
	((Module*)object_)->setProgram(prgId);
}
byte ModuleDevice::program() {
	return ((Module*)object_)->program();
}

#ifdef PLAYER_EDIT_MODE
void ModuleDevice::makeCommandImpl(int command, Stream* stream, va_list args) {
	switch (command) {
	case ModuleCommands::CmdSetUint8:
	case ModuleCommands::CmdSetFloat8:
		stream->writeByte((byte)va_arg(args, int));		// ctrlId
		stream->writeByte((byte)va_arg(args, int));		// value
		break;
	case ModuleCommands::CmdSetFloat:
		stream->writeByte(va_arg(args, int));			// ctrlId
		stream->writeFloat(va_arg(args, float));		// value
		break;
	case CmdSetNote:
		stream->writeByte((byte)va_arg(args, int));		// note
		stream->writeByte((byte)va_arg(args, int));		// velocity
		break;
	case CmdSetProgram:
		stream->writeByte((byte)va_arg(args, int));		// program id
		break;
	}
}
int ModuleDevice::getCommandSize(byte* cmd) {
	var length = 1;
	switch (*cmd) {
	case ModuleCommands::CmdSetUint8:
	case ModuleCommands::CmdSetFloat8:
		length += sizeof(byte);
		break;
	case ModuleCommands::CmdSetFloat:
		length += sizeof(float);
		break;
	case ModuleCommands::CmdSetNote:
		length += 2 * sizeof(byte);
		break;
	case CmdSetProgram:
		length += sizeof(byte);
		break;
	}
	return length;
}
int ModuleDevice::writeToStream(Stream* stream) {
	int start = stream->length();
	stream->writeByte(datablockId_);
	((Module*)object_)->writeToStream(stream);
	return stream->length() - start;
}
#endif

int ModuleDevice::compareToModule(COLLECTION_ARGUMENTS) {
	var mdl1 = ((ModuleDevice*)value)->object();
	return (int)((size_t)mdl1 - (size_t)key.p);
}

Map* ModuleDevice::loadSoundbanks(const char* soundbankPath, SynthAdapter* synthAdapter) {
	var soundbanks = NEW_(Map, sizeof(int), MAP_USE_REF, Map::hashingInt, Collection::compareInt); soundbanks->hasRefKey(false);
	byte* bytes = NULL, * end = NULL;
	// try to read file
	var bytesRead = File::read(soundbankPath, &bytes);
	if (bytesRead != -1) {
		var bptr = bytes;
		end = bytes + bytesRead;
		while (bptr < end) {
			// read device type
			var deviceType = READ(bptr, byte);
			// create dummy device
			var dev = (ModuleDevice*)synthAdapter->createDevice(deviceType);
			var sb = dev->module()->createSoundbank();
			sb->initializeFromStream(bptr);
			soundbanks->put(deviceType, sb);
			DEL_(dev);
		}
	}
	FREE(bytes);
	return soundbanks;
}