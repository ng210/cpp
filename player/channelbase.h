#ifndef __ABSTRACTPLAYER_CHANNEL_H
#define __ABSTRACTPLAYER_CHANNEL_H

#include "playerdef.h"
#include "iadapter.h"
#include "sequencebase.h"

NS_FW_BASE_USE
NS_PLAYER_BEGIN

/******************************************************************************
* Track control flags
*****************************************************************************/
enum Player_Flags : UINT8 {
	// 00-7F: loop count
	Player_Flg_Active = 0x80
};

class ChannelBase {
protected: PROP_R(size_t, id);
protected: PROP_R(void*, target);
protected: PROP_R(IAdapter&, adapter);
protected: PROP_R(SequenceBase*, sequence);
protected: PROP_R(size_t, status);
protected: PROP_R(size_t, currentTick);
protected: PROP_R(size_t, tick);
protected: PROP_R(size_t, loopCount);

	ChannelBase();
public:
	ChannelBase(size_t id);
	ChannelBase(size_t id, const IAdapter& adapter);
	//ChannelBase(size_t id, void* target, const IAdapter& adapter, SequenceBase* sequence);
	virtual ~ChannelBase();

	void assign(void* target, const IAdapter& adapter, SequenceBase* sequence, size_t status = 0);
	bool isActive();
	void setActive(bool flag = true);
	void setLooping(size_t count = 0);
	int reset();

	virtual int run(size_t ticks) = 0;
};

//typedef ChannelBase* (PLAYER_CHANNEL_CREATE)(void);

NS_PLAYER_END

#endif