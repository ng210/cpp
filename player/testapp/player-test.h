#ifndef PLAYER_TEST_H
#define PLAYER_TEST_H

#include "test.h"
#include "utils/task.h"
#include "collection/parray.h"
#include "player/src/device-ctrl.h"
#include "player/testapp/cons-device-ctrl.h"
//#include "player/src/player-ext.h"
//#include "player/src/player-device-ext.h"

NS_FW_BASE_USE
using namespace PLAYER;

class PlayerTest;
class PlayerTestTask : public Task {
	int step_;

	Value values_[10];
	Input inputs_[10];
	InputCtrl ctrls_[10];

	PROP_R(ConsDevice*, consDevice);
	PROP_R(ConsDeviceExt*, consDeviceExt);
	PROP_R(ConsDeviceCtrl*, consDeviceCtrl);
	PROP_R(PlayerDevice*, masterDevice);
	PROP_R(PlayerDeviceExt*, masterDeviceExt);
	PROP_R(PlayerExt*, playerExt);

	PROP_R(PlayerTest*, test);

	void testInputCtrl();
	void testDeviceCtrl();
	void testDeviceCtrlExportImport();
	void testDeviceCtrlImportLayout();
	void testInputCtrlSizing();
	
	static ACTION mainAction_;
public:
	PlayerTestTask(PlayerTest* test);
	~PlayerTestTask();
};

class TestApp;
class PlayerTest : public Test {
	private: PROP_R(TestApp*, app);
	private: PlayerTestTask* task_;
public:
	PlayerTest(TestApp* app);
	~PlayerTest();

	void log(const char* text, ...);
	void dump(const byte* const data, int length, int width = 8);
	Ctrl* area();

	void runAll(int& totalPassed, int& totalFailed);
};

#endif