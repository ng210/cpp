#ifndef __PLAYER_SEQUENCE_H
#define __PLAYER_SEQUENCE_H

#include "collection/array.h"
#include "playerdef.h"
#include "frame.h"
#include "abstractadapter.h"

NS_FW_BASE_USE
NS_PLAYER_BEGIN

class Sequence {
protected: Array frames_;
public:
	Sequence() : frames_(sizeof(Frame)) {};
	~Sequence();
	Frame* addFrame(int time);
	Frame* frame(int ix);

	int toStream(AbstractAdapter* adapter, Buffer* buffer);

#ifdef _DEBUG
	void print(AbstractAdapter* adapter);
#endif

};

NS_PLAYER_END

#endif