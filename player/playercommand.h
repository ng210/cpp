#ifndef __PLAYER_COMMAND_H
#define __PLAYER_COMMAND_H

#include "playerdef.h"

NS_PLAYER_BEGIN

typedef unsigned char COMMAND_ARGUMENT;

#pragma pack(push, 1)
typedef struct PLAYER_COMMAND_S {
	unsigned short delta;
	unsigned char code;
	unsigned char argc;
	unsigned char args;
} PLAYER_COMMAND;
#pragma pack(pop)

typedef union PLAYER_COMMAND_U_ {
	PLAYER_COMMAND_S* s;
	unsigned char* p;
} PLAYER_COMMAND_U;


class PlayerCommand {
public:
	size_t delta_;
	size_t code_;
	COMMAND_ARGUMENT* args_;

	//PlayerCommand(size_t delta, size_t code, COMMAND_ARGUMENT* args);
	PlayerCommand(size_t delta, size_t code, COMMAND_ARGUMENT* args);
	~PlayerCommand();
};

NS_PLAYER_END

#endif