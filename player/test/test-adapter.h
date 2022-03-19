#include "adapter.h"
#include "device.h"

using namespace PLAYER;

typedef enum TestAdapterCommands {
	CmdSetText = 2,
	CmdSetInk,
	CmdMoveTo
} TestAdapterCommand;

typedef enum TestAdapterDevices {
	DevCons
} TestAdapterDevices;

class TestAdapter : public Adapter {
public:
	TestAdapter();
	~TestAdapter();

	AdapterInfo* info();
	Device* createDeviceImpl(int deviceType, byte** initData);
	byte* processCommand(Channel* channel, byte command);
	byte* makeCommand(byte command, Sequence* sequence, byte* cursor);

public:
	static AdapterInfo Info;

	static Adapter* creator(Player* player);
	static void initializer(void* args);
};

class Cons : public Device {
public:
    void setRefreshRate(float fps) {}
    Cons();
	void putText(char* text);
	void move(int x, int y);
	void setInk(int c);
};