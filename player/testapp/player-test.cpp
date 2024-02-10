#include "player/testapp/testapp.h"
#include "base/memory.h"
#include "player/testapp/player-test.h"

#define START_VALUE 45

#pragma region Player test task

PlayerTestTask::PlayerTestTask(PlayerTest* test) : Task() {
	test_ = test;
	context_ = this;
	action(PlayerTestTask::mainAction_);

	#pragma region InputCtrl resources
	value_ = START_VALUE;
	input_ = NEW_(Input, &value_);
	input_->setup(10, 100, 1);
	inputCtrl_ = NEW_(InputCtrl);
	inputCtrl_->create(test_->area(), "Input1");
	DWORD colors[] = { 0x00802000, 0x00804010, 0x00e0c0c0, 0x00f0e0e0 };
	inputCtrl_->setColors(colors);
	inputCtrl_->ctrlId(12);
	inputCtrl_->input(input_);
	inputCtrl_->type(InputCtrlType::Knob);
	inputCtrl_->setSize(140);
	inputCtrl_->move(10, 10);
	//inputCtrl_->show(SW_HIDE);
	#pragma endregion

	#pragma region Player resources
	masterDevice_ = PlayerDevice::create(NULL);
	playerExt_ = NEW_(PlayerExt, masterDevice_->player());
	PlayerDeviceExt::registerExtensionCreator();
	ConsDeviceExt::registerExtensionCreator();
	var consAdapter = NEW_(ConsAdapter);
	playerExt_->player()->addAdapter(consAdapter);
	masterDeviceExt_ = (PlayerDeviceExt*)playerExt_->getDeviceExtension(masterDevice_);
	#pragma endregion

	#pragma region DeviceCtrl resources
	//var cons = &test_->app()->logCtrl();
	//cons->create(test_->area(), "Console");
	consDevice_ = (ConsDevice*)consAdapter->createDevice(ConsDevices::DeviceCons, playerExt_->player());
	consDeviceExt_ = (ConsDeviceExt*)playerExt_->getDeviceExtension(consDevice_);
	consDeviceCtrl_ = NEW_(ConsDeviceCtrl, consDeviceExt_);
	consDeviceCtrl_->create(test_->area(), "ConsDevice");
	consDeviceCtrl_->move(100, 10);
	//...
	#pragma endregion
}

PlayerTestTask:: ~PlayerTestTask() {
	DEL_(inputCtrl_);
	DEL_(input_);

	DEL_(consDeviceCtrl_);
	//DEL_((LogCtrl*)consDevice_->object());
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
	task->test_->totalFailed_ = 0; task->test_->totalPassed_ = 0;
	
	task->testInputCtrl();

	task->testDeviceCtrl();

	//task->testDeviceCtrlExportImport();
	Sleep(3000);
	task->testDeviceCtrlImportLayout();

	//task->test_->displayFinalResults();

	return 1;
}

void PlayerTestTask::testInputCtrl() {
	test_->log("Input control tests\r\n");
	test_->failed_ = 0; test_->passed_ = 0;
	inputCtrl_->show(SW_SHOW);
	// 1. Check Input control
	test_->assert("Should create an Input with correct value", input_ != NULL && input_->value() == &value_ && input_->value()->b == value_.b);
	var pV = input_->value();

	// 2. user sets minimum
	test_->log("Set input control to the minimum value!\r\n");
	while (state_ == TASK_STATE::TaskRunning && pV->b != input_->min.b) Sleep(10);

	// 3. user sets maximum
	test_->log("Set input control to the maximum value!\r\n");
	while (state_ == TASK_STATE::TaskRunning && pV->b != input_->max.b) Sleep(10);

	// 4. user sets middle value
	test_->log("Set input control to middle value!\r\n");
	var mid = (input_->max.b + input_->min.b) / 2;
	while (state_ == TASK_STATE::TaskRunning && (pV->b > mid)) Sleep(10);
	EnableWindow(inputCtrl_->hWnd(), false);

	test_->assert("Tests completed!", true);
	test_->log("\r\n");

	wait(100);
	if (state_ == TASK_STATE::TaskRunning) {
		inputCtrl_->show(SW_HIDE);
	}
}

void PlayerTestTask::testDeviceCtrl() {
	test_->failed_ = 0; test_->passed_ = 0;
	test_->log("ConsDevice control tests\r\n");

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

	wait(10);
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
	test_->log("Expected\r\n");
	test_->dump(expected.data(), expected.length(), 16);

	consDeviceExt_->writeToStream(&received);
	test_->log("Received\r\n");
	test_->dump(received.data(), received.length(), 16);

	var isMatch = true;
	test_->log("Binary comparison\r\n");
	expected.rewind();
	received.rewind();
	for (var i = 0; i < expected.length(); i++) {
		var ca = expected.readByte();
		var cb = received.readByte();
		if (ca != cb) {
			test_->log(" - %04X: %02X %02X\r\n", i, ca, cb);
			isMatch = false;
			break;
		}
	}
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
	LayoutItem item;
	item.ctrlId = 0;
	item.position = { 0, 0 };
	strncpy(item.label, 16, "Pos X");
	item.type = InputCtrlType::Knob;
	stream.writeBytes((byte*)&item, sizeof(LayoutItem));
	item.ctrlId = 1;
	item.position = { 40, 0 };
	strncpy(item.label, 16, "Pos Y");
	item.type = InputCtrlType::Knob;
	stream.writeBytes((byte*)&item, sizeof(LayoutItem));
	item.ctrlId = 2;
	item.position = { 80, 0 };
	strncpy(item.label, 16, "Color");
	item.type = InputCtrlType::Knob;
	stream.writeBytes((byte*)&item, sizeof(LayoutItem));
	stream.rewind();
	consDeviceCtrl_->initFromStream(&stream);

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