#include "consoleapp.h"
#include "test-adapter.h"
#include "base/memory.h"
#include "channel.h"
#include "stream.h"


Cons::Cons() {}

void Cons::putText(char* text) {
    var cons = getConsole();
    cons->printf("%s", text);
}
void Cons::setInk(int c) {
    var cons = getConsole();
    cons->setcolor(c);
}
void Cons::move(int x, int y) {
    var cons = getConsole();
    cons->movexy(x, y);
}

AdapterInfo TestAdapter::Info = { "TestAdapter", 123, TestAdapter::creator, TestAdapter::initializer };

TestAdapter::TestAdapter() {
    devices_.init(4);
}

TestAdapter::~TestAdapter() {

}

AdapterInfo* TestAdapter::info() { return &TestAdapter::Info; }

Device* TestAdapter::createDeviceImpl(int deviceType, byte** initData) {
    Device* device = NULL;
    switch (deviceType) {
    case TestAdapterDevices::DevCons:
        device = NEW_(Cons);
    }
    return device;
}

byte* TestAdapter::processCommand(Channel* channel, byte command) {
    var device = (Cons*)channel->device();
    var sequence = channel->sequence();
    var cursor = channel->cursor();
    switch (command) {
    case CmdMoveTo:
        int x, y;
        x = READ(cursor, byte);
        if (x > 127) x -= 256;
        y = READ(cursor, byte);
        if (y > 127) y -= 256;
        device->move(x, y);
        break;
    case CmdSetText:
        device->putText((char*)cursor);
        cursor += fmw::strlen((char*)cursor) + 1;
        break;
    case CmdSetInk:
        var c = READ(cursor, byte);
        device->setInk(c);
        break;
    }
    channel->cursor(cursor);
	return NULL;
}

Stream* TestAdapter::makeCommand(byte command, ...) {
    var stream = NEW_(Stream, 2);
    va_list args;
    va_start(args, command);
    stream->writeByte(command);
    switch ((PlayerCommands)command) {
    case CmdSetText:
        stream->writeString(va_arg(args, char*));
        break;
    case CmdSetInk:
        stream->writeByte(va_arg(args, int));
        break;
    case CmdMoveTo:
        stream->writeByte(va_arg(args, int));
        stream->writeByte(va_arg(args, int));
        break;
    }
    return stream;
}
int TestAdapter::getCommandArgsSize(byte command, byte* stream) {
    var length = 0;
    switch ((PlayerCommands)command) {
    case CmdSetText:
        length += fmw::strlen((const char*)stream) + 1;
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

Adapter* TestAdapter::creator(Player* player) {
    return NEW_(TestAdapter);
}

void TestAdapter::initializer(void* args) {
    //getConsole()->clearscreen();
}
