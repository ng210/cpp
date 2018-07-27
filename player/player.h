#ifndef __PLAYER_ABSTRACT_PLAYER_H
#define __PLAYER_ABSTRACT_PLAYER_H

#include "playerdef.h"
#include "collection/parray.h"
#include "abstractchannel.h"
#include "iadapter.h"

NS_FW_BASE_USE
NS_PLAYER_BEGIN

typedef struct USER_DATA_BLOCK_ITEM_ {
	size_t length;
	void* userDataBlock;
} USER_DATA_BLOCK_ITEM;

class PlayerAdapter;
class Player {
	friend class PlayerAdapter;
protected:	PROP_R(PArray*, adapters);
protected:	PROP_R(PArray*, sequences);
protected:	PROP_R(Array*, userDataBlocks);
protected:	PROP_R(PArray*, targets);
protected:	PROP_R(PArray*, channels);
protected:	PROP_R(AbstractChannel*, masterChannel);
protected:	PROP_R(float, refreshRate);

protected:
	void addChannel(AbstractChannel* chn);
	//void addTarget(void* object, IAdapter* adapter);
public:
	Player();
	virtual ~Player();

	void addAdapter(IAdapter* adapter, void* data = NULL);
	void addSequence(PLAYER_SEQUENCE sequence);
	void addUserDataBlock(size_t length, void* userData);
	void addTarget(void* object, IAdapter* adapter);

	/// TODO: turn into player command

	virtual void run(size_t ticks);
	void refreshRate(float ticksPerSecond);

	static PLAYER_CHANNEL_CREATE* createChannel;
};

NS_PLAYER_END

#endif