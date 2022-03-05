#ifndef __PLAYER_CHANNELBIN_H
#define __PLAYER_CHANNELBIN_H

#include "channelbase.h"
#include "sequencebin.h"

NS_PLAYER_BEGIN

class ChannelBin : public ChannelBase {
	PLAYER_FRAME_BIN_U cursor_;
public:
	ChannelBin(size_t);
	int run(size_t ticks);
};

NS_PLAYER_END

#endif