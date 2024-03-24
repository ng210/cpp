#include "console/consoleapp.h"
#include "cons-device.h"

using namespace PLAYER;

ConsDevice::ConsDevice(Adapter* adapter, Player* player, void* cons) : Device(adapter, player, cons) {
    type(ConsDevices::DeviceCons);
    inputCount_ = 3;
    x_ = 0;
    y_ = 0;
    ink_ = ConsoleColors::gray;
    var inputs = NEW_(ConsInputs);
    inputs_ = (Input*)inputs;
    inputs->x.setValue(&x_); inputs->x.setup(0, 20, 1);
    inputs->y.setValue(&y_); inputs->y.setup(0, 20, 1);
    inputs->ink.setValue(&ink_); inputs->ink.setup(ConsoleColors::black, ConsoleColors::gray, 1);
}

ConsDevice::~ConsDevice() {
    DEL_((ConsInputs*)inputs_);
}

void ConsDevice::initialize(byte** pData) {
    if (pData != NULL) {
        var x = READ(*pData, byte);
        var y = READ(*pData, byte);
        var col = READ(*pData, byte);
        goTo(x, y);
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
        moveTo(x, y);
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
    ((IConsole*)object_)->printf("%s", text);
}
void ConsDevice::setInk(int c) {
    ink_ = c;
    ((IConsole*)object_)->setcolor(c);
}
void ConsDevice::moveTo(int x, int y) {
    x_.b += x; y_.b += y;
    ((IConsole*)object_)->movexy(x, y);
}

void ConsDevice::goTo(int x, int y) {
    x_ = x; y_ = y;
    ((IConsole*)object_)->movexy(x, y);
}
