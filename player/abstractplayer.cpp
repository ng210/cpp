#include "abstractplayer.h"
#include "base/memory.h"

NS_FW_BASE_USE
NS_PLAYER_BEGIN

AbstractPlayer::AbstractPlayer() {
	targets_ = NEW_(PArray);
	sequences_ = NEW_(PArray);
}
AbstractPlayer::~AbstractPlayer() {
	ARRAY_FOREACH(targets_, DEL_((Target*)value));
	DEL_(targets_);
	ARRAY_FOREACH(sequences_, DEL_((PArray*)value));
	DEL_(sequences_);
}

void AbstractPlayer::addTarget(void* object, AbstractAdapter* adapter) {
	targets_->add(NEW_(Target, object, adapter));
	adapter->prepareObject(object);
}
NS_PLAYER_END