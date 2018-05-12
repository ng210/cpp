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

//#pragma pack(push, 1)
//typedef struct PLAYER_COMMAND_ {
//	WORD delta;
//	BYTE code;
//} PLAYER_COMMAND;
//#pragma pack(pop)

//typedef union PLAYER_SEQUENCE_ {
//	PLAYER_COMMAND* obj;
//	BYTE* ptr;
//} PLAYER_SEQUENCE;

typedef BYTE* PLAYER_COMMAND;

typedef void* PLAYER_SEQUENCE;

#define Player_Cmd_end 0xff

#endif