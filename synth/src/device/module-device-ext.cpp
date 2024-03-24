#include "player/src/player.h"
#include "synth/src/device/module-device-ext.h"

using namespace SYNTH;

ModuleDeviceExt::ModuleDeviceExt(Device* device) : DeviceExt(device) {
}

void ModuleDeviceExt::makeCommandImpl(int command, Stream* stream, va_list args) {
	switch (command) {
	case ModuleCommands::CmdSetUint8:
	case ModuleCommands::CmdSetFloat8:
		stream->writeByte((byte)va_arg(args, int));		// ctrlId
		stream->writeByte((byte)va_arg(args, int));		// value
		break;
	case ModuleCommands::CmdSetFloat:
		stream->writeByte((byte)va_arg(args, int));			// ctrlId
		stream->writeFloat((float)va_arg(args, double));		// value
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

int ModuleDeviceExt::getCommandSize(byte* cmd) {
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

void ModuleDeviceExt::writeToStream(Stream* stream) {
	DeviceExt::writeToStream(stream);
	var dev = (ModuleDevice*)device_;
	stream->writeByte(dev->datablockId());
	stream->writeByte(dev->preset());
}
