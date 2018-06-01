#include "player.h"
#include "target.h"
#include "base/memory.h"
#include "playeradapter.h"

NS_FW_BASE_USE
NS_PLAYER_BEGIN

PLAYER_CHANNEL_CREATE* Player::createChannel = NULL;

Player::Player() {
	targets_ = NEW_(PArray);
	adapters_ = NEW_(PArray);
	PlayerAdapter* playerAdapter = NEW_(PlayerAdapter);
	adapters_->add(playerAdapter);
	userData_ = NEW_(Array, sizeof(PLAYER_USER_DATA));
	addTarget(this, playerAdapter);
	sequences_ = NEW_(PArray);
	channels_ = NEW_(PArray);
	masterChannel_ = NULL;
	//framesPerSecond_ = 25;
	//ticksPerFrame_ = 10;
	refreshRate_ = 25 * 10;
}
Player::~Player() {
	DEL_(((Target*)targets_->getAt(0))->adapter);
	ARRAY_FOREACH(targets_, DEL_((Target*)value));
	DEL_(targets_);
	ARRAY_FOREACH(channels_, DEL_((AbstractChannel*)value));
	DEL_(channels_);
	DEL_(sequences_);
	DEL_(adapters_);
	DEL_(userData_);
}

void Player::addTarget(void* object, AbstractAdapter* adapter) {
	targets_->add(NEW_(Target, object, adapter));
	adapter->prepareObject(object);
}

//void AbstractPlayer::addSequence(PLAYER_SEQUENCE sequence) {
//	sequences_->add(sequence);
//}
void Player::addSequence(PLAYER_SEQUENCE sequence) {
	sequences_->add(sequence);
}

void Player::addUserData(UINT32 length, void* data) {
	PLAYER_USER_DATA userData;
	userData.length = length;
	userData.data = data;
	userData_->add(&userData);
}

void Player::addChannel(AbstractChannel* chn) {
	if (channels_->length() == 0) {
		masterChannel_ = chn;
	}
	channels_->add(chn);
}

void Player::run(size_t ticks) {
	// run master channel
	if (masterChannel_->isActive()) {
		if (masterChannel_->run(ticks)) {
			// reset
			ARRAY_FOREACH(channels(), ((AbstractChannel*)value)->setActive(false));
		}
	}
	if (masterChannel_->isActive()) {
		// if master is still active run every other channel
		for (UINT32 i = 1; i < channels_->length(); i++) {
			AbstractChannel* chn = (AbstractChannel*)channels_->getAt(i);
			if (chn->isActive()) {
				chn->run(ticks);
			}
		}
	}
}

void Player::refreshRate(float ticksPerSecond) {
	refreshRate_ = ticksPerSecond;
}
NS_PLAYER_END