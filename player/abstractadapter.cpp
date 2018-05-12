#include "abstractadapter.h"
#include "base/debug.h"

NS_PLAYER_BEGIN

PLAYER_COMMAND AbstractAdapter::createEndCommand() {
	BYTE* cmd = MALLOC(BYTE, sizeof(BYTE));
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

//int AbstractAdapter::dumpCommand(PLAYER_COMMAND command, Buffer& buffer) {
//	buffer.append(command, sizeof(BYTE));
//	int length = 1;
//	switch (command[0]) {
//	case Player_Cmd_end:
//		break;
//	default:
//		LOG("Invalid command: %02X\n", command[0]);
//		break;
//	}
//	return length;
//}

NS_PLAYER_END