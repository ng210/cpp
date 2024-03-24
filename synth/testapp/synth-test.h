#ifndef PLAYER_TEST_H
#define PLAYER_TEST_H

#include "test.h"
#include "utils/task.h"
#include "collection/parray.h"
//#include "player/src/device-ctrl.h"
#include "synth/src/ui/synth-ui.h"
//#include "synth/src/ui/synth-ctrl.h"
//#include "synth/src/ui/mixer-ctrl.h"

NS_FW_BASE_USE
using namespace PLAYER;

class SynthTest;
class SynthTestTask : public Task {
	private: PROP_R(SynthTest*, test);
	private: PROP_R(BassCtrl*, bassCtrl);
	
	static ACTION mainAction_;
public:
	SynthTestTask(SynthTest* test);
	~SynthTestTask();
};

class TestApp;
class SynthTest : public Test {
	private: PROP_R(TestApp*, app);
	private: SynthTestTask* task_;
	private: PROP_R(SynthAdapter*, synthAdapter);
	private: PROP_R(Player*, player);
	private: PROP_R(PlayerDevice*, masterDevice);
	private: PROP_R(PlayerDeviceExt*, masterDeviceExt);
	private: PROP_R(PlayerExt*, playerExt);
	//private: PROP_R(PArray*, deviceExts);
	private: PROP_R(PArray*, controls);
	private: PROP_R(Mixer8x4*, mixer);
	byte* createBinary();
public:
	SynthTest(TestApp* app);
	~SynthTest();

	void log(const char* text, ...);
	void dump(const byte* const data, int length, int width = 8);
	Ctrl* area();

	void runAll(int& totalPassed, int& totalFailed);
};

#endif