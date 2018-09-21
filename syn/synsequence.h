#ifndef __EDITOR_SEQUENCE_H
#define __EDITOR_SEQUENCE_H

#include "syndef.h"
#include "player/sequence.h"

NS_SYN_BEGIN
NS_PLAYER_USE

class SynSequence : public Sequence {
public:
	//SynSequence();
	//~SynSequence();
	//Frame* addFrame(int time);
	//Frame* frame(int ix);

	//static int toStream(Sequence* sequence, IAdapter* adapter, Buffer* buffer);
	char* info(IAdapter* adapter);
};

NS_SYN_END

#endif