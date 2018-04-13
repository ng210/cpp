#include "playercommand.h"
//#include "base/memory.h"
//#include <varargs.h>

NS_PLAYER_BEGIN

////PlayerCommand::PlayerCommand(size_t delta, size_t code, COMMAND_ARGUMENT* args) {
//	delta_ = delta;
//	code_ = code;
//	args_ = args;
//}

PlayerCommand::PlayerCommand(size_t delta, size_t code, COMMAND_ARGUMENT* args) {
	delta_ = delta;
	code_ = code;
	args_ = args;
	//args_ = argCount > 0 ? args_ = MALLOC(COMMAND_ARGUMENT, argCount) : NULL;
	//va_list args;
	//va_start(args, argCount);
	//for (size_t i = 0; i < argCount; i++) {
	//	COMMAND_ARGUMENT arg = va_arg(args, COMMAND_ARGUMENT);
	//	args_[i] = arg;
	//}
	//va_end(args);
}

PlayerCommand::~PlayerCommand() {
}

NS_PLAYER_END