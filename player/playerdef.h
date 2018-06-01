#ifndef __PLAYER_DEF_H
#define __PLAYER_DEF_H

#define NS_PLAYER ply
#define NS_PLAYER_BEGIN namespace NS_PLAYER {
#define NS_PLAYER_END }
#define NS_PLAYER_USE using namespace NS_PLAYER;

#include "basedef.h"
#include "base/memory.h"
#include "collection/parray.h"

NS_FW_BASE_USE

typedef UINT8* PLAYER_COMMAND;

typedef void* PLAYER_SEQUENCE;

#define Player_Cmd_end 0xff

#endif