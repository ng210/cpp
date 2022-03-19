#include "consoleapp.h"
#include "test-adapter.h"
#include "base/memory.h"
#include "channel.h"


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

byte* TestAdapter::makeCommand(byte command, Sequence* sequence, byte* cursor) {
    //var stream = new Stream(128);
    //stream.writeUint8(command);
    //var inputStream = null;
    //if (arguments[1] instanceof Ps.Sequence) inputStream = arguments[1].stream;
    //else if (arguments[1] instanceof Stream) inputStream = arguments[1];
    //switch (command) {
    //case TestAdapter.SetText:
    //    if (inputStream) {
    //        stream.writeString(inputStream.readString(arguments[2]));
    //    }
    //    else {
    //        stream.writeString(arguments[1]);
    //    }
    //    break;
    //case TestAdapter.SetInk:
    //    if (inputStream) {
    //        stream.writeUint8(inputStream.readUint8(arguments[2]));
    //    }
    //    else {
    //        stream.writeUint8(arguments[1]);
    //    }
    //    break;
    //}
	return NULL;
}

Adapter* TestAdapter::creator(Player* player) {
    return NEW_(TestAdapter);
}

void TestAdapter::initializer(void* args) {
    //getConsole()->clearscreen();
}
