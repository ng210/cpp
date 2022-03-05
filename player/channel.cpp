#include "channel.h"

NS_PLAYER_BEGIN

Channel::Channel(size_t id) {
	id_ = id;
}

int Channel::run(size_t ticks) {
	int ret = 0;
	bool restart;
	Frame* frame = NULL;
	PLAYER_COMMAND_U command;
loopstart:
	restart = false;
	while (true) {
		frame = ((Sequence*)sequence_)->frame(cursor_);
		if (frame->delta_ < currentTick_) {
			currentTick_ -= frame->delta_;
			PArray* commands = frame->commands();
			for (UINT32 i = 0; i < commands->length(); i++) {
				command.bytes = (UINT8*)commands->getAt(i);
				//printf("#%03llu, channel:%02llu, command:%02x\n", tick_, id_, command[0]);
				if (command.s->cmdId != Player_Cmd_end) {
					adapter_.processCommand(target_, command);
				} else {
					cursor_ = 0;
					restart = true;
					ret = reset();
					goto loopstart;
				}
			}
			cursor_++;
		} else {
			currentTick_++;
			tick_++;
			break;
		}
	}
	return ret;
}

NS_PLAYER_END