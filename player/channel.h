#ifndef __PLAYER_CHANNEL_H
#define __PLAYER_CHANNEL_H

#include "channelbase.h"
#include "sequence.h"

NS_PLAYER_BEGIN

class Channel : public ChannelBase {
	int cursor_;
public:
	Channel(size_t);
	int run(size_t ticks);
};

NS_PLAYER_END

#endif