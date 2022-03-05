#ifndef __PLAYER_SEQUENCE_H
#define __PLAYER_SEQUENCE_H

#include "playerdef.h"
#include "collection/parray.h"
#include "sequencebase.h"
#include "frame.h"

NS_FW_BASE_USE
NS_PLAYER_BEGIN

class Sequence : public SequenceBase {
protected: PROP_R(PArray, frames);
public:
	Sequence();
	Sequence(UINT8* bytes);
	~Sequence();

	Frame* frame(int ix);

#ifdef _EDITOR	// Editor extensions
	Frame* addFrame(int time);

	int fromStream(const IAdapter& adapter, UINT8* stream);
	int toStream(const IAdapter& adapter, UINT8* stream);
#endif
};

NS_PLAYER_END

#endif