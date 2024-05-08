#include "player/testapp/testapp.h"
#include "base/memory.h"
#include "player/testapp/player-test.h"

#define START_VALUE 45

#pragma region Player test task

PlayerTestTask::PlayerTestTask(PlayerTest* test) : Task() {
	test_ = test;
	context_ = this;
	action(PlayerTestTask::mainAction_);
	step_ = 0;

	#pragma region InputCtrl resources
	char lbl[16];
	var x = 0;
	for (var ii = 0; ii < 10; ii++) {
		InputBase* input;
		if (ii < 3) {
			values_[ii] = ii;
			input = NEW_(Input);
			input->setup(0, 9, 1);
		}
		else if (ii < 6) {
			values_[ii] = ii/9;
			input = NEW_(InputF);
			input->setup(0.0f, 1.0f, 0.01f);
		}
		else {
			values_[ii] = ii;
			input = NEW_(InputF8);
			input->setup(0, 9, 1);
		}
		input->setValue(&values_[ii]);
		inputs_[ii] = input;
		sprintf_s(lbl, 16, "In%2d", ii+1);
		ctrls_[ii].create(test_->area(), lbl);
		ctrls_[ii].gap = 2;
		DWORD colors[] = { 0x00802000, 0x00804010, 0x00e0c0c0, 0x00f0e0e0 };
		ctrls_[ii].setColors(colors);
		ctrls_[ii].ctrlId(ii+1);
		ctrls_[ii].input(inputs_[ii]);
		ctrls_[ii].type(InputCtrlType::Knob);
		ctrls_[ii].setScale((int)(200.0f * ii / 9.0f));
		ctrls_[ii].move(x, 10);
		x += ctrls_[ii].rect().right + 2;
	}

	values_[0] = START_VALUE;
	inputs_[0]->setup(10, 100, 1);
	DWORD colors[] = { 0x00802000, 0x00804010, 0x00e0c0c0, 0x00f0e0e0 };
	ctrls_[0].setColors(colors);
	#pragma endregion

	#pragma region Player resources
	masterDevice_ = PlayerDevice::create(NULL);
	playerExt_ = NEW_(PlayerExt, masterDevice_->player());
	PlayerDeviceExt::registerExtensionCreator();
	ConsDeviceExt::registerExtensionCreator();
	var consAdapter = playerExt_->player()->addAdapter(NEW_(ConsAdapter));
	masterDeviceExt_ = (PlayerDeviceExt*)playerExt_->getDeviceExtension(masterDevice_);
	#pragma endregion

	#pragma region DeviceCtrl resources
	//var cons = &test_->app()->logCtrl();
	//cons->create(test_->area(), "Console");
	consDevice_ = (ConsDevice*)consAdapter->createDevice(ConsDevices::DeviceCons, playerExt_->player());
	byte preset[] = {
		3,
		'D','e','f','a', 'u','l','t',  0,   0,  0,  0,  0,   0,  0,  0,  0,
		'S','t','a','r', 't',' ','p','o', 'i','n','t',  0,   0,  0,  0,  0,
		'T','a','b','u', 'l','a','t','o', 'r',  0,  0,  0,   0,  0,  0,  0,

		0, 0, ConsoleColors::green,
		1, 1, ConsoleColors::blue,
		10, 0, ConsoleColors::yellow
	};
	var pb = NEW_(PresetBank, 3, preset);
	consDevice_->setPresetBank(pb);
	consDeviceExt_ = (ConsDeviceExt*)playerExt_->getDeviceExtension(consDevice_);
	consDeviceCtrl_ = NEW_(ConsDeviceCtrl, consDeviceExt_);
	consDeviceCtrl_->create(test_->area(), "ConsDevice");
	consDeviceCtrl_->move(100, 10);
	consDeviceCtrl_->show(false);
	#pragma endregion
}

PlayerTestTask:: ~PlayerTestTask() {
	DEL_(consDeviceCtrl_);
	//DEL_((LogCtrl*)consDevice_->object());
	DEL_(consDevice_->presetBank());
	DEL_(consDevice_);
	DEL_(consDeviceExt_);

	DEL_(playerExt_);
	DEL_(masterDevice_);
	DEL_(masterDeviceExt_);
	Player::cleanUp();
	PlayerExt::cleanUp();
}

DWORD PlayerTestTask::mainAction_(void* context) {
	var task = (PlayerTestTask*)context;
	var exitCode = 0;
	switch (task->step_) {
		case 0:
			task->test_->totalFailed_ = 0; task->test_->totalPassed_ = 0;
			break;
		case 1:
			task->testInputCtrl();
			break;
		case 2:
			task->testDeviceCtrl();
			break;
		case 3:
			task->testDeviceCtrlExportImport();
			break;
		case 4:
			Sleep(2000);
			break;
		case 5:
			task->testDeviceCtrlImportLayout();
			Sleep(4000);
			break;
		case 6:
			task->consDeviceCtrl_->show(false);
			task->testInputCtrlSizing();
			exitCode = 1;
			break;

	}
	task->step_++;
	return exitCode;
}

void PlayerTestTask::testInputCtrl() {
	test_->log("Input control tests\r\n");
	test_->failed_ = 0; test_->passed_ = 0;
	ctrls_[0].show(SW_SHOW);
	// 1. Check Input control
	test_->assert("Should create an Input with correct value", inputs_[9]->value() == &values_[9] && inputs_[9]->value()->b == values_[9].b);
	var pV = inputs_[9]->value();

	// 2. user sets minimum
	test_->log("Set input control #10 to the minimum value!\r\n");
	while (state_ == TASK_STATE::TaskRunning && pV->b != inputs_[9]->min.b) Sleep(10);

	// 3. user sets maximum
	test_->log("Set input control to the maximum value!\r\n");
	while (state_ == TASK_STATE::TaskRunning && pV->b != inputs_[9]->max.b) Sleep(10);

	// 4. user sets middle value
	test_->log("Set input control to middle value!\r\n");
	var mid = (inputs_[9]->max.b + inputs_[9]->min.b) / 2;
	while (state_ == TASK_STATE::TaskRunning && (pV->b > mid)) Sleep(10);
	EnableWindow(ctrls_[0].hWnd(), false);

	test_->assert("Tests completed!", true);
	test_->log("\r\n");

	wait(100);
}

void PlayerTestTask::testDeviceCtrl() {
	test_->failed_ = 0; test_->passed_ = 0;
	test_->log("ConsDevice control tests\r\n");
	consDeviceCtrl_->show(true);
	test_->assert("Should create a ConsDevice control", consDeviceCtrl_ != NULL);

	var dev = consDeviceCtrl_->deviceExt()->device();
	var isMatch = true;
	for (var ii = 0; ii < consDeviceCtrl_->inputCount(); ii++) {
		var ctrl = consDeviceCtrl_->getInput(ii);
		isMatch = isMatch && (dev->getInput(ii)->value() == ctrl->input()->value());
		if (!isMatch) break;
	}
	test_->assert("Should map inputs correctly", consDeviceCtrl_ != NULL && isMatch);

	test_->assert("Tests completed!", true);
	test_->log("\r\n");

	consDevice_->goTo(1, 1);

	var input = consDeviceCtrl_->getInput(0)->input();
	var pV = input->value();
	test_->log("Set input control #1 to the minimum value!\r\n");
	while (state_ == TASK_STATE::TaskRunning && pV->b != input->min.b) Sleep(10);

	if (state_ == TASK_STATE::TaskRunning) {
		consDeviceCtrl_->onClose();
	}
}

void PlayerTestTask::testDeviceCtrlExportImport() {
	test_->log("Device control export tests\r\n");
	test_->failed_ = 0; test_->passed_ = 0;

	int x = 10, y = 12, ink = ConsoleColors::green;
	consDevice_->goTo(x, y);
	consDevice_->setInk(ink);

	Stream expected(1024);
	Stream received(1024);

	expected.writeByte(ConsDevices::DeviceCons)->writeByte(x)->writeByte(y)->writeByte(ink);
	consDeviceExt_->writeToStream(&received);
	var isMatch = test_->binaryCompare(received.data(), received.length(), expected.data(), expected.length());
	test_->assert("Should be binary identical", isMatch);

	consDevice_->goTo(4, 6);
	consDevice_->setInk(ConsoleColors::yellow);
	var p = received.data() + 1;	// skip device type
	consDevice_->initialize(&p);
	test_->assert("Should load binary data into device",
		consDevice_->getValue(ConsInputsId::ConsInputX)->b == x &&
		consDevice_->getValue(ConsInputsId::ConsInputY)->b == y &&
		consDevice_->getValue(ConsInputsId::ConsInputColor)->b == ink);

	test_->log("Tests completed!");
	test_->log("\r\n");
}

void PlayerTestTask::testDeviceCtrlImportLayout() {
	test_->log("Device control import layout\r\n");
	test_->failed_ = 0; test_->passed_ = 0;

	// build layout data	
	Stream stream(1024);
	stream.writeByte(0)->writeString("Pos X")->writeWord( 0)->writeWord( 0)->writeByte(InputCtrlType::Knob | InputCtrlNoLabel)->writeByte(100);
	stream.writeByte(1)->writeString("Pos Y")->writeWord(58)->writeWord( 0)->writeByte(InputCtrlType::VPotmeter | InputCtrlNoValue)->writeByte(100);
	stream.writeByte(2)->writeString("Color")->writeWord( 4)->writeWord(64)->writeByte(InputCtrlType::HPotmeter)->writeByte(100);
	stream.rewind();
	consDeviceCtrl_->initLayoutFromStream(&stream);
	SYSPR(InvalidateRect(consDeviceCtrl_->hWnd(), NULL, true));

	var input = consDeviceCtrl_->getInput(0)->input();
	var pV = input->value();
	test_->log("Set input control #1 to the minimum value!\r\n");
	while (state_ == TASK_STATE::TaskRunning && pV->b != input->min.b) Sleep(10);

	if (state_ == TASK_STATE::TaskRunning) {
		consDeviceCtrl_->onClose();
	}

	test_->assert("Tests completed!", true);
	test_->log("\r\n");
}

void PlayerTestTask::testInputCtrlSizing() {
	while (state_ == TASK_STATE::TaskRunning && values_[0].b != inputs_[0]->min.b) Sleep(10);

	test_->assert("Tests completed!", true);
	test_->log("\r\n");
}

#pragma endregion

#pragma region Player tests
PlayerTest::PlayerTest(TestApp* app) {
	app_ = app;
	task_ = NULL;
}

PlayerTest::~PlayerTest() {
	if (task_ != NULL) {
		task_->stop();
		DEL_(task_);
	}
}

void PlayerTest::log(const char* text, ...) {
	va_list args;
	va_start(args, text);
	app_->logCtrl().vprintf(text, args);
	va_end(args);
}

void PlayerTest::dump(const byte* const data, int length, int width) {
	app_->logCtrl().dump(data, length, width);
}

Ctrl* PlayerTest::area() {
	return app_->testArea();
}

void PlayerTest::runAll(int& totalPassed, int& totalFailed) {
	task_ = NEW_(PlayerTestTask, this);
	task_->run();
}
#pragma endregion