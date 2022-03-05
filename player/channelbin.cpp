#include "channelbin.h"

NS_PLAYER_BEGIN

ChannelBin::ChannelBin(size_t id) {
	id_ = id;
}

int ChannelBin::run(size_t ticks) {
	int ret = 0;
	bool restart;
	do {
		restart = false;
		while (cursor_.frame->delta < currentTick_) {
			currentTick_ -= cursor_.frame->delta;
			if (cursor_.frame->cmd.s->cmdId != Player_Cmd_end) {
				cursor_.bytes += sizeof(cursor_.frame->delta);
				adapter_.processCommand(target_, cursor_.frame->cmd);
			} else {
				cursor_ = ((SequenceBin*)sequence_)->stream();
				restart = true;
				ret = reset();
				break;
			}
		}
		currentTick_++;
		tick_++;
	} while (restart);
	return ret;
}

NS_PLAYER_END
