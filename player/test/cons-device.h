#ifndef __CONS_DEVICE_H
#define __CONS_DEVICE_H

#include "player/src/device.h"

using namespace PLAYER;

typedef enum ConsCommands {
	CmdSetText = 2,
	CmdSetInk,
	CmdMoveTo
} ConsCommands;

typedef enum ConsInputs {
	ConsInputX,
	ConsInputY,
	ConsInputColor
} ConsInputs;

class ConsDevice : public Device {
public:
	ConsDevice(Adapter* adapter);
	~ConsDevice();

	void initialize(byte** pData = NULL);

	void setValue(int id, Value value);
	Value* getValue(int id);

	int run(int ticks);
	//void setRefreshRate(float fps);
	void processCommand(byte cmd, byte*& cursor);

	// console object
private:
	int x_, y_, ink_;
public:
	void putText(char* text);
	void move(int x, int y);
	void setInk(int c);
};
#endif