#ifndef __PLAYER_CHANNEL_H
#define __PLAYER_CHANNEL_H

#include "Player.h"
#include "AbstractAdapter.h"
#include "collection/Array.h"

NS_FW_BASE_USE

NS_PLAYER_BEGIN

class Channel : public Object {
protected: PROP_R(	Player*, player);
protected: PROP_R(	Target*, target);
protected: PROP_R(	Array*, sequence);		// Array<PlayerCommand>
protected: PROP_R(	size_t, cursor);
protected: PROP_R(	size_t, status);
protected: PROP_R(	size_t, currentTick);
					size_t frame_;
public:
	Channel() {};
	Channel(Player* player, Target* target, Array* sequence);
	virtual ~Channel();

	void set(Player* player, Target* target, Array* sequence, size_t status = 0);
	void run(size_t ticks);

	void setActive(bool flag = true);
	void setLooping(bool flag = true);
	bool isActive();

	String id_;
};

NS_PLAYER_END

#endif