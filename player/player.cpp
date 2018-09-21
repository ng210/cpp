#include "player.h"
#include "target.h"
#include "base/memory.h"
#include "playeradapter.h"

NS_FW_BASE_USE
NS_PLAYER_BEGIN

PLAYER_CHANNEL_CREATE* Player::createChannel = NULL;

Player::Player() {
	adapters_ = NEW_(PArray);
	sequences_ = NEW_(PArray);
	userDataBlocks_ = NEW_(Array, sizeof(USER_DATA_BLOCK_ITEM));
	targets_ = NEW_(PArray);
	channels_ = NEW_(PArray);
	masterChannel_ = NULL;

	PlayerAdapter* playerAdapter = NEW_(PlayerAdapter);
	adapters_->add(playerAdapter);
	addTarget(this, playerAdapter);
	//targets_->add(playerAdapter->createTarget(this));
	refreshRate_ = 25 * 10;
}
Player::~Player() {
	ARRAY_FOREACH(channels_, DEL_((AbstractChannel*)value));
	DEL_(channels_);
	IAdapter* adapter = ((Target*)targets_->getAt(0))->adapter;
	DEL_(adapter);
	ARRAY_FOREACH(targets_, DEL_((Target*)value));
	DEL_(targets_);
	DEL_(userDataBlocks_);
	DEL_(sequences_);
	DEL_(adapters_);
}

//void Player::addAdapter(IAdapter* adapter, void* data) {
//	targets_->add(adapter);
//	//adapter->initialize((UINT8*)data);
//}

void Player::addSequence(PLAYER_SEQUENCE sequence) {
	sequences_->add(sequence);
	if (masterChannel_ == NULL && sequences_->length() == 1) {
		AbstractChannel* chn = Player::createChannel();
		chn->init(0, (Target*)targets_->getAt(0), sequence, Player_Flg_Active);
		addChannel(chn);
	}
}

void Player::addUserDataBlock(size_t length, void* userData) {
	USER_DATA_BLOCK_ITEM item;
	item.length = length;
	item.userDataBlock = userData;
	userDataBlocks_->add(&item);
}

void Player::addTarget(void* object, IAdapter* adapter) {
	Target* target = NEW_(Target, object, adapter);
	targets_->add(target);
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