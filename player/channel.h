#ifndef __PLAYER_CHANNEL_H
#define __PLAYER_CHANNEL_H

#include "abstractchannel.h"

//NS_FW_BASE_USE

NS_PLAYER_BEGIN

class Player;
class Channel : public AbstractChannel {

public:
	int run(size_t ticks);
};

NS_PLAYER_END

#endif