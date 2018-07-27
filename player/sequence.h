#ifndef __PLAYER_SEQUENCE_H
#define __PLAYER_SEQUENCE_H

#include "collection/array.h"
#include "playerdef.h"
#include "frame.h"
#include "iadapter.h"

NS_FW_BASE_USE
NS_PLAYER_BEGIN

class Sequence {
protected:
	Array frames_;
public:
	Sequence();
	~Sequence();
	Frame* addFrame(int time);
	Frame* frame(int ix);
};

NS_PLAYER_END

#endif