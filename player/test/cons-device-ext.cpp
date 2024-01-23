#include "console/consoleapp.h"
#include "base/memory.h"
#include "player/src/sequence.h"
#include "player/src/player.h"
#include "player/test/cons-adapter.h"
#include "player/test/cons-device-ext.h"

using namespace PLAYER;

DeviceExt* ConsDeviceExt::consDeviceExtCreator_(Device* device) {
    DeviceExt* deviceExt = NULL;
    switch (device->type()) {
        case ConsDevices::DeviceCons: deviceExt = NEW_(ConsDeviceExt, device);
    }
    return deviceExt;
}

ConsDeviceExt::ConsDeviceExt(Device* device) : DeviceExt(device) {
    inputs_ = (Input*)&consDeviceInputs_;
    consDeviceInputs_.x.value = device->getValue(0); consDeviceInputs_.x.initialize(0, 20, 1);
    consDeviceInputs_.y.value = device->getValue(1); consDeviceInputs_.y.initialize(0, 20, 1);
    consDeviceInputs_.ink.value = device->getValue(2); consDeviceInputs_.ink.initialize(ConsoleColors::black, ConsoleColors::gray, 1);
}

ConsDeviceExt::~ConsDeviceExt() {
}

Input* ConsDeviceExt::getInput(int id) {
    Input* input = NULL;
    if (id >= 0 && id < device_->inputCount()) {
        input = &inputs_[id];
    }
    return input;
}

void ConsDeviceExt::makeCommandImpl(int command, Stream* stream, va_list args) {
    switch ((ConsCommands)command) {
    case CmdSetText:
        char* str; str = va_arg(args, char*);
        int len; len = fmw::strlen(str);
        strncpy((char*)stream, len, str);
        stream += len;
        break;
    case CmdSetInk:
        *stream++ = va_arg(args, byte);
        break;
    case CmdMoveTo:
        *stream++ = va_arg(args, byte);
        *stream++ = va_arg(args, byte);
        break;
    }
}

int ConsDeviceExt::getCommandSize(byte* cmd) {
    var length = 1;
    switch (*cmd) {
    case CmdSetText:
        length += fmw::strlen((char*)++cmd) + 1;
        break;
    case CmdSetInk:
        length++;
        break;
    case CmdMoveTo:
        length += 2;
        break;
    }
    return length;
}

Sequence* ConsDeviceExt::createDefaultSequence() {
    var seq = NEW_(Sequence);
    seq->writeHeader();
    
    seq->writeDelta(0);
    seq->writeCommand(ConsCommands::CmdMoveTo)->writeByte(0)->writeByte(1);
    seq->writeCommand(ConsCommands::CmdSetInk)->writeByte(ConsoleColors::green);
    seq->writeCommand(ConsCommands::CmdSetText)->writeString("Hello World!");
    seq->writeCommand(PlayerCommands::CmdEOF);

    seq->writeDelta(40);
    seq->writeCommand(ConsCommands::CmdMoveTo)->writeByte(0)->writeByte(-1);
    seq->writeCommand(ConsCommands::CmdSetInk)->writeByte(ConsoleColors::black);
    seq->writeCommand(ConsCommands::CmdSetText)->writeString("Hello World!");
    seq->writeCommand(PlayerCommands::CmdEOF);

    seq->writeDelta(40);
    seq->writeCommand(PlayerCommands::CmdEOS);
    return seq;
}

void ConsDeviceExt::registerExtensionCreator() {
    ConsAdapter adapter;
    var key = (adapter.getInfo()->id << 8) + ConsDevices::DeviceCons;
    PlayerExt::addDeviceExtension(key, ConsDeviceExt::consDeviceExtCreator_);
}