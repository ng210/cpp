#ifndef __PLAYER_CHANNEL_H
#define __PLAYER_CHANNEL_H

#include "collection/parray.h"
#include "player.h"

//NS_FW_BASE_USE

NS_PLAYER_BEGIN

class Player;
class Channel {
protected: PROP_R(Player*, player);
protected: PROP_R(Target*, target);
protected: PROP_R(PArray*, sequence);		// Array<PlayerCommand>
protected: PROP_R(int, cursor);
protected: PROP_R(size_t, status);
protected: PROP_R(size_t, currentTick);
protected: PROP_R(size_t, tick);
protected: PROP_R(size_t, loopCount);
public:
	Channel() {};
	Channel(Player* player, size_t id, Target* target, PArray* sequence);
	virtual ~Channel();

	void set(Player* player, size_t id, Target* target, PArray* sequence, size_t status = 0);
	void run(size_t ticks);

	void setActive(bool flag = true);
	void setLooping(size_t count = 0);
	bool isActive();

	size_t id_;
};

NS_PLAYER_END

#endif