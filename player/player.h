#ifndef __PLAYER_ABSTRACT_PLAYER_H
#define __PLAYER_ABSTRACT_PLAYER_H

#include "playerdef.h"
#include "collection/parray.h"
#include "abstractchannel.h"
#include "abstractadapter.h"

NS_FW_BASE_USE
NS_PLAYER_BEGIN

typedef struct PLAYER_USER_DATA_ {
	UINT32 length;
	void* data;
} PLAYER_USER_DATA;

class Player {
protected:	PROP_R(PArray*, adapters);
protected:	PROP_R(PArray*, sequences);
protected:	PROP_R(Array*, userData);
protected:	PROP_R(PArray*, targets);
protected:	PROP_R(PArray*, channels);
protected:	PROP_R(AbstractChannel*, masterChannel);
protected:	PROP_R(float, refreshRate);

public:
	Player();
	virtual ~Player();

	void addSequence(PLAYER_SEQUENCE sequence);
	void addUserData(UINT32 length, void* data);

	void addTarget(void* target, AbstractAdapter* adapter);
	void addChannel(AbstractChannel* chn);

	virtual void run(size_t ticks);
	void refreshRate(float ticksPerSecond);

	static PLAYER_CHANNEL_CREATE* createChannel;
};

NS_PLAYER_END

#endif