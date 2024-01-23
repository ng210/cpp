#include "console/consoleapp.h"
#include "cons-adapter.h"
#include "cons-device.h"

using namespace PLAYER;

ConsDevice::ConsDevice(Adapter* adapter) : Device(adapter, NULL) {
    type(ConsDevices::DeviceCons);
    inputCount_ = 3;
}

ConsDevice::~ConsDevice() {
}

void ConsDevice::initialize(byte** pData) {
    if (pData != NULL) {
        preset_ = READ(*pData, byte);
        var x = READ(*pData, byte);
        var y = READ(*pData, byte);
        var col = READ(*pData, byte);
        move(x, y);
        setInk(col);
    }
}

void ConsDevice::setValue(int id, Value value) {
    switch (id) {
        case ConsInputX: x_ = value.i; break;
        case ConsInputY: y_ = value.i; break;
        case ConsInputColor: ink_ = value.i; break;
    }
}

Value* ConsDevice::getValue(int id) {
    Value* v = NULL;
    switch (id) {
        case ConsInputX: v = (Value*)&x_; break;
        case ConsInputY: v = (Value*)&y_; break;
        case ConsInputColor: v = (Value*)&ink_; break;
    }
    return v;
}

int ConsDevice::run(int ticks) {
    return 1;
}

void ConsDevice::processCommand(byte cmd, byte*& cursor) {
    switch (cmd) {
    case CmdMoveTo:
        int x, y;
        x = READ(cursor, byte);
        if (x > 127) x -= 256;
        y = READ(cursor, byte);
        if (y > 127) y -= 256;
        move(x, y);
        break;
    case CmdSetText:
        putText((char*)cursor);
        cursor += (byte)fmw::strlen((char*)cursor) + 1;
        break;
    case CmdSetInk:
        var c = READ(cursor, byte);
        setInk(c);
        break;
    }
}

void ConsDevice::putText(char* text) {
    var cons = getConsole();
    cons->printf("%s", text);
}
void ConsDevice::setInk(int c) {
    ink_ = c;
    var cons = getConsole();
    cons->setcolor(c);
}
void ConsDevice::move(int x, int y) {
    x_ = x; y_ = y;
    var cons = getConsole();
    cons->movexy(x, y);
}

