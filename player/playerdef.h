#ifndef __PLAYER_DEF_H
#define __PLAYER_DEF_H

#define NS_PLAYER ply
#define NS_PLAYER_BEGIN namespace NS_PLAYER {
#define NS_PLAYER_END }
#define NS_PLAYER_USE using namespace NS_PLAYER;

#include "basedef.h"
//#include "base/memory.h"
//#include "collection/parray.h"

//typedef UINT8* PLAYER_COMMAND;

#define Player_Cmd_end 0xff

typedef struct ADAPTER_COMMAND_BASE_ {
	UINT8 cmdId;
} ADAPTER_COMMAND_BASE;

typedef struct PLAYER_COMMAND_S_ : ADAPTER_COMMAND_BASE {
	UINT8* args;
} PLAYER_COMMAND_S;

union PLAYER_COMMAND_U {
	PLAYER_COMMAND_S* s;
	UINT8* bytes;
};

#endif
