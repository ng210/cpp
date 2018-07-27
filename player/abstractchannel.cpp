#include <stdio.h>
#include "abstractchannel.h"

NS_PLAYER_BEGIN

AbstractChannel::AbstractChannel(size_t id, Target* target, PLAYER_SEQUENCE sequence) {
	init(id, target, sequence);
}

AbstractChannel::~AbstractChannel() {

}

void AbstractChannel::init(size_t id, Target* target, PLAYER_SEQUENCE sequence, size_t status) {
	assign(target, sequence, status);
	id_ = id;
}

void AbstractChannel::reset() {
	cursor_ = 0;
	loopCount_ = 0;
	currentTick_ = 0;
	tick_ = 0;
}

void AbstractChannel::assign(Target* target, PLAYER_SEQUENCE sequence, size_t status) {
	reset();
	target_ = target;
	sequence_ = sequence;
	status_ = status & 0x80;
	setLooping(status & 0x7f);
}

void AbstractChannel::setActive(bool flag) {
	flag ? status_ |= Player_Flg_Active : status_ &= ~Player_Flg_Active;
}

void AbstractChannel::setLooping(size_t count) {
	loopCount_ = count;
}

bool AbstractChannel::isActive() {
	return status_ & Player_Flg_Active;
}

NS_PLAYER_END

//void AbstractChannel::run(size_t ticks) {
//	bool restart;
//	do {
//		restart = false;
//		PLAYER_COMMAND* command = (PLAYER_COMMAND*)sequence_->getAt(cursor_);
//		if (currentTick_ < command->delta) {
//			//printf("#%03llu, channel:%02llu, tick:%02llu, delta:%03u\n", tick_, id_, currentTick_, command->delta);
//			// advance tick counter
//			currentTick_++;
//			tick_++;
//			break;
//		}
//		currentTick_ = 0;	//-= command->delta;
//		do {
//			//printf("#%03llu, channel:%02llu, command:%02x\n", tick_, id_, command->code);
//			if (command->code != (unsigned char)Player_Cmd_end) {
//				target_->adapter->processCommand(target_->object, command);
//			}
//			else {
//				tick_ = 0;
//				if (loopCount_ != 0) {
//					if (this == player_->masterChannel()) {
//						// reset
//						ARRAY_FOREACH(player_->channels(), ((Channel*)value)->setActive(false));
//					}
//					// restart sequence
//					cursor_ = 0;
//					currentTick_ = 0;
//					loopCount_--;
//					restart = true;
//					break;
//				}
//				else {
//					//if (this == player_->masterChannel()) {
//					//	// deactivate channel
//					setActive(false);
//					//}
//					//else {
//					//	// remove channel
//					//	player_->channels_->splice(this->id_, 1, 0);
//					//}
//					break;
//				}
//			}
//			cursor_++;
//			command = (PLAYER_COMMAND*)sequence_->getAt(cursor_);
//		} while (command->delta == 0);
//		if (!restart) {
//			currentTick_++;
//			tick_++;
//		}
//	} while (restart);
//}