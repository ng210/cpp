#include "abstractadapter.h"
#include "base/debug.h"

NS_PLAYER_BEGIN

PLAYER_COMMAND AbstractAdapter::createEndCommand() {
	UINT8* cmd = MALLOC(UINT8, sizeof(UINT8));
	cmd[0] = Player_Cmd_end;
	return cmd;
}

#ifdef _DEBUG
char* AbstractAdapter::logCommand(PLAYER_COMMAND command) {
	char* str = NULL;
	switch (command[0]) {
	case Player_Cmd_end:
		str = strdup("end");
		break;
	default:
		char* str = strdup("???");
		break;
	}
	return str;
}
#endif

NS_PLAYER_END