#ifndef __PLAYER_SEQUENCEBIN_H
#define __PLAYER_SEQUENCEBIN_H

#include "playerdef.h"
#include "sequencebase.h"

NS_FW_BASE_USE
NS_PLAYER_BEGIN

class SequenceBin : public SequenceBase {
protected: PROP_R(PLAYER_FRAME_BIN_U, stream);
public:
	SequenceBin(UINT8* stream);
	virtual ~SequenceBin();
};

NS_PLAYER_END

#endif
