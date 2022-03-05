#include "sequencebin.h"

NS_PLAYER_BEGIN

SequenceBin::SequenceBin(UINT8* stream) {
	this->stream_.bytes = stream;
}

NS_PLAYER_END