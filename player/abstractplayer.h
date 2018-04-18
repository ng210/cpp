#ifndef __PLAYER_ABSTRACT_PLAYER_H
#define __PLAYER_ABSTRACT_PLAYER_H

#include "playerdef.h"
#include "playercommand.h"
#include "collection/parray.h"
#include "abstractadapter.h"

NS_FW_BASE_USE
NS_PLAYER_BEGIN

class AbstractPlayer : public AbstractAdapter {
protected:	AbstractAdapter* adapter_;
protected:	PROP_R(PArray*, targets);
protected:	PROP_R(PArray*, sequences);
protected:	size_t framesPerSecond_;
protected:	size_t ticksPerFrame_;
protected:	PROP_R(size_t, refreshRate);

public:
	AbstractPlayer();
	virtual ~AbstractPlayer();

	void addTarget(void* target, AbstractAdapter* adapter);

	virtual void addSequence(void* stream) = 0;
	virtual void run(size_t ticks) = 0;
	virtual size_t getCurrentFrame() = 0;
};

NS_PLAYER_END

#endif