//#include <stdarg.h>
#include "frame.h"
#include "base/memory.h"

NS_PLAYER_BEGIN

Frame::Frame() {
	delta_ = 0;
	commands_ = NEW_(PArray);
}
Frame::~Frame() {
	empty();
	DEL_(commands_);
}

void Frame::empty() {
	// commands_->clear()
	ARRAY_FOREACH(commands_, FREE(value););
}

#ifdef _EDITOR	// _EDITOR extensions
void Frame::addCommand(UINT8* cmd) {
	commands_->add(cmd);
}
PLAYER_COMMAND Frame::makeCommand(int code, int argc, ...) {
	UINT8* cmd = MALLOC(UINT8, (1 + argc) * sizeof(UINT8));
	cmd[0] = code;
	va_list args;
	va_start(args, argc);
	for (int i = 0; i < argc; i++) {
		cmd[1 + i] = va_arg(args, UINT8);
	}
	va_end(args);
	return cmd;
}
#endif

NS_PLAYER_END