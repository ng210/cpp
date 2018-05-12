#include <stdarg.h>
#include "frame.h"
#include "base/memory.h"

NS_PLAYER_BEGIN

Frame::Frame() {
	delta_ = 0;
	commands_ = NEW_(PArray);
}
Frame::~Frame() {
	//DEL_(commands_);
}

void Frame::destroy() {
	empty();
	DEL_(commands_);
}

void Frame::addCommand(PLAYER_COMMAND cmd) {
	commands_->add(cmd);
}

void Frame::empty() {
	ARRAY_FOREACH(commands_, FREE(value););
}

PLAYER_COMMAND Frame::makeCommand(int code, int argc, ...) {
	BYTE* cmd = MALLOC(BYTE, (1 + argc) * sizeof(BYTE));
	cmd[0] = code;
	va_list args;
	va_start(args, argc);
	for (int i = 0; i < argc; i++) {
		cmd[1 + i] = va_arg(args, BYTE);
	}
	va_end(args);
	return cmd;
}

NS_PLAYER_END