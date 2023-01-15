#include "console/consoleapp.h"
#include "cons-adapter.h"
#include "cons-device.h"

using namespace PLAYER;


ConsDevice::ConsDevice(Adapter* adapter) : Device(NULL, adapter) {
    type(ConsDevices::DeviceCons);
    initData_ = NULL;
}
ConsDevice::~ConsDevice() {
    //FREE(initData_);
}


void ConsDevice::initialize(byte** pData) {
    initData_ = *pData;
    var x = READ(*pData, byte);
    var y = READ(*pData, byte);
    var col = READ(*pData, byte);
    move(x, y);
    setInk(col);
}

int ConsDevice::run(int ticks) {
    return 1;
}
void ConsDevice::setRefreshRate(float fps) {

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

#ifdef PLAYER_EDIT_MODE
void ConsDevice::makeCommandImpl(int command, byte*& stream, va_list args) {
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
int ConsDevice::getCommandSize(byte cmd, byte* args) {
    var length = 1;
    switch (cmd) {
    case CmdSetText:
        length += fmw::strlen((char*)args) + 1;
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
int ConsDevice::writeToStream(Stream* stream) {
    var len = Device::writeToStream(stream);
    stream->writeBytes(initData_, 3);
    len++;
    return len;
}
#endif

void ConsDevice::putText(char* text) {
    var cons = getConsole();
    cons->printf("%s", text);
}
void ConsDevice::setInk(int c) {
    var cons = getConsole();
    cons->setcolor(c);
}
void ConsDevice::move(int x, int y) {
    var cons = getConsole();
    cons->movexy(x, y);
}

