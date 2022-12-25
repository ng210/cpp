#include "base/memory.h"
#include "module-device.h"
#include "../module/module.h"

using namespace SYNTH;

ModuleDevice::ModuleDevice(void* object, Adapter* adapter) : Device(object, adapter) {

}

void ModuleDevice::initialize(byte** pData) {
	((Module*)object_)->initialize(pData);
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

void ModuleDevice::setControl(byte ctrlId, byte value) {
	var ctrl = ((Module*)object_)->getControl(ctrlId);
	ctrl->value.b = value;
}
void ModuleDevice::setControl(byte ctrlId, float value) {
	var ctrl = ((Module*)object_)->getControl(ctrlId);
	ctrl->value.f = value;
}


#ifdef PLAYER_EDIT_MODE
void ModuleDevice::makeCommandImpl(int command, byte*& stream, va_list args) {
	switch (command) {
	case ModuleCommands::CmdSetUint8:
	case ModuleCommands::CmdSetFloat8:
		*stream++ = (byte)va_arg(args, int);	// ctrlId
		*stream++ = (byte)va_arg(args, int);	// value
		break;
	case ModuleCommands::CmdSetFloat:
		*stream++ = (byte)va_arg(args, int);	// ctrlId
		*stream = (byte)va_arg(args, float);	// value
		stream += sizeof(float);
		break;
	}
}
int ModuleDevice::getCommandSize(byte cmd, byte* args) {
	var length = 1;
	switch (cmd) {
	case ModuleCommands::CmdSetUint8:
	case ModuleCommands::CmdSetFloat8:
		length += sizeof(byte);
		break;
	case ModuleCommands::CmdSetFloat:
		length += sizeof(float);
		break;
	}
	return length;
}
#endif

