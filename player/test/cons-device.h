#ifndef __CONS_DEVICE_H
#define __CONS_DEVICE_H

#include "device.h"

using namespace PLAYER;

typedef enum ConsCommands {
	CmdSetText = 2,
	CmdSetInk,
	CmdMoveTo
} ConsCommands;

class ConsDevice : public Device {
public:
	ConsDevice(Adapter* adapter);
	~ConsDevice();

	void initialize(byte** pData = NULL);

	int run(int ticks);
	void setRefreshRate(float fps);
	void processCommand(byte cmd, byte*& cursor);

#ifdef PLAYER_EDIT_MODE
	void makeCommandImpl(int command, byte*& stream, va_list args);
	int getCommandSize(byte cmd, byte* args);
	int writeToStream(Stream* stream);
#endif
	void putText(char* text);
	void move(int x, int y);
	void setInk(int c);
};
#endif