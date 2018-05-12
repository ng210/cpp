#ifndef __ABSTRACTPLAYER_CHANNEL_H
#define __ABSTRACTPLAYER_CHANNEL_H

#include "playerdef.h"
#include "collection/parray.h"
#include "target.h"

//#include "abstractadapter.h"

NS_FW_BASE_USE

NS_PLAYER_BEGIN

/******************************************************************************
* Track control flags
*****************************************************************************/
enum Player_Flags : BYTE {
	// 00-7F: loop count
	Player_Flg_Active = 0x80
};

class AbstractPlayer;
class AbstractChannel {
//protected: PROP_R(AbstractPlayer*, player);
protected: PROP_R(Target*, target);
protected: PROP_R(PLAYER_SEQUENCE, sequence);
protected: PROP_R(int, cursor);
protected: PROP_R(size_t, status);
protected: PROP_R(size_t, currentTick);
protected: PROP_R(size_t, tick);
protected: PROP_R(size_t, loopCount);
protected: PROP_R(size_t, id);
public:
	AbstractChannel() {};
	AbstractChannel(/*AbstractPlayer* player,*/ size_t id, Target* target, PLAYER_SEQUENCE sequence);
	virtual ~AbstractChannel();

	virtual void reset();
	virtual void init(/*AbstractPlayer* player,*/ size_t id, Target* target, PLAYER_SEQUENCE sequence, size_t status = 0);
	virtual void assign(/*AbstractPlayer* player,*/ Target* target, PLAYER_SEQUENCE sequence, size_t status = 0);
	virtual int run(size_t ticks) = 0;

	void setActive(bool flag = true);
	void setLooping(size_t count = 0);
	bool isActive();
};

typedef AbstractChannel* (PLAYER_CHANNEL_CREATE)(void);

NS_PLAYER_END

#endif