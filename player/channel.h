#ifndef __PLAYER_CHANNEL_H
#define __PLAYER_CHANNEL_H

#include "abstractchannel.h"

NS_PLAYER_BEGIN

class Channel : public AbstractChannel {
public:
	int run(size_t ticks);
};

NS_PLAYER_END

#endif