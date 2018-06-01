#include "channel.h"
#include "player.h"
#include "sequence.h"
#include <stdio.h>

NS_PLAYER_BEGIN

int Channel::run(size_t ticks) {
	bool restart;
	int reset = 0;
	do {
		restart = false;
		Frame* frame = ((Sequence*)sequence_)->frame(cursor_);
		if (currentTick_ < frame->delta_) {
			//printf("#%03llu, channel:%02llu, tick:%02llu, delta:%03u\n", tick_, id_, currentTick_, frame->delta);
			// advance tick counter
			currentTick_++;
			tick_++;
			break;
		}
		currentTick_ = 0;	//-= command->delta;
		PArray* commands = frame->commands();
		for (UINT32 i = 0; i < commands->length(); i++) {
			UINT8* command = (UINT8*)commands->getAt(i);
			//printf("#%03llu, channel:%02llu, command:%02x\n", tick_, id_, command[0]);
			if (command[0] != Player_Cmd_end) {
				target_->adapter->processCommand(target_->object, command);
			} else {
				tick_ = 0;
				if (loopCount_ != 0) {
					// restart sequence
					reset = 1;
					cursor_ = 0;
					currentTick_ = 0;
					loopCount_--;
					restart = true;
					break;
				} else {
					setActive(false);
					break;
				}
			}
		}
		if (!restart) {
			cursor_++;
			currentTick_++;
			tick_++;
		}
	} while (restart);
	return reset;
}
NS_PLAYER_END