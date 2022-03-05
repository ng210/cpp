#include "player.h"
#include "target.h"
#include "base/memory.h"
#include "adapterlib.h"
#include "playeradapter.h"

#include "channel.h"
#include "channelbin.h"

NS_FW_BASE_USE
NS_PLAYER_BEGIN

Player::Player() {
	init();
}

void Player::init() {
	// create array of adapters, sequences and user data blocks
	adapters_ = NEW_(PArray);
	sequences_ = NEW_(PArray);
	userDataBlocks_ = NEW_(Array, sizeof(USER_DATA_BLOCK_ITEM));
	// create array of targets
	targets_ = NEW_(PArray, 64);
	// create pool of channels
	channels_ = NEW_(PArray, 64);
	for (UINT32 i = 1; i <= channels_->length(); i++) {
#ifdef _EDITOR
		channels_->setAt(i, NEW_(Channel, i));
#else
		channels_->setAt(i, NEW_(ChannelBin, i));
#endif
	}
	masterChannel_ = (ChannelBase*)channels_->getAt(0);

	// add player adapter
	PlayerAdapter* playerAdapter = NEW_(PlayerAdapter);
	adapters_->add(playerAdapter);
	// add this player as target
	addTarget(this);
	//targets_->add(playerAdapter->createTarget(this));
	refreshRate_ = 25 * 10;
}

Player::~Player() {
	ARRAY_FOREACH(channels_, DEL_((ChannelBase*)value));
	DEL_(channels_);
	IAdapter* adapter = ((Target*)targets_->getAt(0))->adapter;
	DEL_(adapter);
	ARRAY_FOREACH(targets_, DEL_((Target*)value));
	DEL_(targets_);
	DEL_(userDataBlocks_);
	DEL_(sequences_);
	DEL_(adapters_);
}

int Player::load(UINT8* data) {
	int ret = 0;
	// get adapters
	// create

	return ret;
}

void Player::addAdapter(IAdapter* adapter, UINT8 userDataBlockId) {
	adapters_->add(adapter);
	USER_DATA_BLOCK_ITEM* data = (USER_DATA_BLOCK_ITEM*)userDataBlocks_->getAt(userDataBlockId);
	if (data != NULL) {
		adapter->prepareContext(data->userDataBlock);
		adapter->createObjects(targets_, data->userDataBlock);
	}
}

void Player::addSequence(SequenceBase* sequence) {
	sequences_->add(sequence);
	if (sequences_->length() == 1) {
		masterChannel_->assign(targets_->getAt(0), PlayerAdapter::getInstance(), sequence, Player_Flg_Active);
	}
}

void Player::addUserDataBlock(size_t length, void* userData) {
	USER_DATA_BLOCK_ITEM item;
	item.length = length;
	item.userDataBlock = userData;
	userDataBlocks_->add(&item);
}

void Player::addTarget(void* target) {
	targets_->add(target);
}

ChannelBase& Player::getFreeChannel() {
	ChannelBase* chn = NULL;
	ARRAY_FOREACH(channels_, if (!((ChannelBase*)value)->isActive()) { chn = (ChannelBase*)value; break; });
	if (chn == NULL) {
#ifdef _EDITOR
		channels_->add(NEW_(Channel, channels_->length()));
#else
		channels_->add(NEW_(ChannelBin, channels_->length()));
#endif
	}
	return *chn;
}

void Player::run(size_t ticks) {
	// run master channel
	if (masterChannel_->isActive()) {
		if (masterChannel_->run(ticks)) {
			// reset
			ARRAY_FOREACH(channels(), ((ChannelBase*)value)->setActive(false));
		}
	}
	if (masterChannel_->isActive()) {
		// if master is still active run every other channel
		for (UINT32 i = 1; i < channels_->length(); i++) {
			ChannelBase* chn = (ChannelBase*)channels_->getAt(i);
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