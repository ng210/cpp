#ifndef __PLAYER_ABSTRACT_PLAYER_H
#define __PLAYER_ABSTRACT_PLAYER_H

#include "iadapter.h"
#include "collection/array.h"
#include "sequencebase.h"
#include "channelbase.h"

NS_FW_BASE_USE
NS_PLAYER_BEGIN

typedef struct USER_DATA_BLOCK_ITEM_ {
	size_t length;
	void* userDataBlock;
} USER_DATA_BLOCK_ITEM;

class Player {
protected:	PROP_R(PArray*, adapters);
protected:	PROP_R(PArray*, sequences);
protected:	PROP_R(Array*, userDataBlocks);
protected:	PROP_R(PArray*, targets);

protected:	PROP_R(PArray*, channels);

protected:	PROP_R(float, refreshRate);
protected:	ChannelBase* masterChannel_;

	void init();
public:
	Player();
	virtual ~Player();

	int load(UINT8* data);

	void addAdapter(IAdapter* adapter, UINT8 userDataBlockId = -1);
	void addSequence(SequenceBase* sequence);
	void addUserDataBlock(size_t length, void* userData);
	void addTarget(void* target);
	ChannelBase& getFreeChannel();

	ChannelBase& masterChannel();

	virtual void run(size_t ticks);
	void refreshRate(float ticksPerSecond);

	/// TODO: _EDITOR extensions

};

NS_PLAYER_END

#endif