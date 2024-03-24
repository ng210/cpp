#ifndef __CONS_DEVICE_H
#define __CONS_DEVICE_H

#include "utils/iconsole.h"
#include "player/src/device.h"
#include "player/test/cons-adapter.h"

NS_FW_BASE_USE
using namespace PLAYER;

typedef enum ConsCommands {
	CmdSetText = 2,
	CmdSetInk,
	CmdMoveTo
} ConsCommands;

typedef enum ConsInputId_ {
	ConsInputX,
	ConsInputY,
	ConsInputColor
} ConsInputsId;

typedef struct ConsInputs_ {
	Input x;
	Input y;
	Input ink;
} ConsInputs;

class ConsDevice : public Device {
	friend class ConsAdapter;
protected:
	ConsDevice(Adapter* adapter, Player* player, void* cons = NULL);
public:	
	~ConsDevice();

	void initialize(byte** pData = NULL);

	void setValue(int id, Value value);
	Value* getValue(int id);

	int run(int ticks);
	//void setRefreshRate(float fps);
	void processCommand(byte cmd, byte*& cursor);

	// console object
private:
	Value x_, y_, ink_;
public:
	void putText(char* text);
	void moveTo(int x, int y);
	void goTo(int x, int y);
	void setInk(int c);
};
#endif