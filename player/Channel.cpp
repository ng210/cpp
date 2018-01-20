#include "channel.h"

NS_PLAYER_BEGIN

Channel::Channel(Player* player, Target* target, Array* sequence) {
	set(player, target, sequence);
}

Channel::~Channel() {

}

void Channel::set(Player* player, Target* target, Array* sequence, size_t status) {
	player_ = player;
	target_ = target;
	sequence_ = sequence;
	status_ = status;
	cursor_ = 0;
	currentTick_ = 0;
	frame_ = 0;
}
void Channel::run(size_t ticks) {
	bool restart;
	do {
		restart = false;
		if (currentTick_ < ((PlayerCommand*)sequence_->get(cursor_))->delta) {
			// advance tick counter
			currentTick_++;
			break;
		}
		currentTick_ = 0;	//-= this.sequence[this.cursor].delta;

		do {
			PlayerCommand* command = (PlayerCommand*)sequence_->get(cursor_);
			if (command->cmd != Player_Cmd_End) {
				target_->adapter->processCommand(target_->object, command);
				currentTick_++;
			} else {
				if ((status_ & Player_Flg_Looping) != 0) {
					if (this == player_->masterChannel()) {
						// reset
						player_->channels_->cleanUp();
						player_->channels_->push(this);
					}
					// restart sequence
					cursor_ = 0;
					currentTick_ = 0;
					restart = true;
					break;
				} else {
					// deactivate channel
					if (this == player_->masterChannel()) {
						setActive(false);
					} /* else {
						// todo remove channel
					} */
					break;
				}
			}
			cursor_++;
		} while (((PlayerCommand*)sequence_->get(cursor_))->delta == 0);
	} while (restart);
}


void Channel::setActive(bool flag) {
	flag ? status_ |= Player_Flg_Active : status_ &= ~Player_Flg_Active;
}

void Channel::setLooping(bool flag) {
	flag ? status_ |= Player_Flg_Looping : status_ &= ~Player_Flg_Looping;
}

bool Channel::isActive() {
	return status_ & Player_Flg_Active;
}

NS_PLAYER_END