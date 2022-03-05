#include <stdio.h>
#include "channelbase.h"

NS_PLAYER_BEGIN

static int dummyAdapter_ = 0;

ChannelBase::ChannelBase() : id_(-1), adapter_((IAdapter&)dummyAdapter_) {
}

ChannelBase::ChannelBase(size_t id) : id_(id), adapter_((IAdapter&)dummyAdapter_) {
}

ChannelBase::ChannelBase(size_t id, const IAdapter& adapter) : id_(id), adapter_((IAdapter&)adapter) {
}

int ChannelBase::reset() {
	int reset = 0;
	tick_ = 0;
	if (loopCount_ != 0) {
		// restart sequence
		currentTick_ = 0;
		loopCount_--;
		reset = 1;
	} else {
		// set inactive
		setActive(false);
	}
	return reset;
}

void ChannelBase::assign(void* target, const IAdapter& iadapter, SequenceBase* sequence, size_t status) {
	reset();
	target_ = target;
	adapter_ = iadapter;
	sequence_ = sequence;
	status_ = status & 0x80;
	setLooping(status & 0x7f);
}

void ChannelBase::setActive(bool flag) {
	flag ? status_ |= Player_Flg_Active : status_ &= ~Player_Flg_Active;
}

void ChannelBase::setLooping(size_t count) {
	loopCount_ = count;
}

bool ChannelBase::isActive() {
	return status_ & Player_Flg_Active;
}

NS_PLAYER_END