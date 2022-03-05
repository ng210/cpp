#ifndef __PLAYER_SEQUENCEBASE_H
#define __PLAYER_SEQUENCEBASE_H

#include "playerdef.h"

NS_FW_BASE_USE
NS_PLAYER_BEGIN

typedef struct PLAYER_FRAME_BIN_ {
	UINT16 delta;
	PLAYER_COMMAND_U cmd;
} PLAYER_FRAME_BIN;

union PLAYER_FRAME_BIN_U {
	PLAYER_FRAME_BIN* frame;
	UINT8* bytes;
};

class SequenceBase {
};


NS_PLAYER_END

#endif
