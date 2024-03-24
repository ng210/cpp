#include "synth/testapp/testapp.h"
#include "base/memory.h"
#include "synth/testapp/synth-test.h"
#include "soundlib/src/soundplayer.h"

#define START_VALUE 45

#define samplingRate 48000

#pragma region Synth test task
SynthTestTask::SynthTestTask(SynthTest* test) : Task() {
	test_ = test;
	context_ = this;
	action(SynthTestTask::mainAction_);
}

SynthTestTask::~SynthTestTask() {
	if (test_->player()->isPlaying()) {
		test_->player()->stop();
	}
}

DWORD SynthTestTask::mainAction_(void* context) {
	var task = (SynthTestTask*)context;
	var player = task->test_->player();

	if (!player->isPlaying()) {
		player->start();
	}

	return 1;
}

#pragma endregion

#pragma region Synth tests
SynthTest::SynthTest(TestApp* app) {
	app_ = app;
	task_ = NULL;

	PlayerDeviceExt::registerExtensionCreator();
	BassDeviceExt::registerExtensionCreator();
	SynthDeviceExt::registerExtensionCreator();
	MixerDeviceExt::registerExtensionCreator();

	SynthUI::registerControlCreators();

	synthAdapter_ = (SynthAdapter*)Player::addAdapter(NEW_(SynthAdapter));
	masterDevice_ = PlayerDevice::create(NULL);
	player_ = masterDevice_->player();
	playerExt_ = NEW_(PlayerExt, player_);
	masterDeviceExt_ = (PlayerDeviceExt*)playerExt_->getDeviceExtension(masterDevice_);
	mixer_ = NULL;

	//deviceExts_ = NEW_(PArray, 16);
	controls_ = NEW_(PArray, 16);
	//deviceExts()->push(masterDeviceExt_);
}

SynthTest::~SynthTest() {
	SoundPlayer::stop();

	if (task_ != NULL) {
		task_->stop();
		DEL_(task_);
	}

	DEL_(playerExt_);
	DEL_(masterDeviceExt_);
	DEL_(masterDevice_);
	//deviceExts_->apply([](COLLECTION_ARGUMENTS) {
	//	var ext = (DeviceExt*)value;
	//	DEL_(ext->device());
	//	DEL_(ext);
	//	return value;
	//});
	//DEL_(deviceExts_);
	controls_->apply([](COLLECTION_ARGUMENTS) {
		var ctrl = (DeviceCtrl*)value;
		DEL_(ctrl->deviceExt());
		DEL_(ctrl);
		return value;
	});
	DEL_(controls_);
	Player::cleanUp();
	PlayerExt::cleanUp();
}

void SynthTest::log(const char* text, ...) {
	va_list args;
	va_start(args, text);
	app_->logCtrl().vprintf(text, args);
	va_end(args);
}

void SynthTest::dump(const byte* const data, int length, int width) {
	app_->logCtrl().dump(data, length, width);
}

Ctrl* SynthTest::area() {
	return app_->testArea();
}

byte* SynthTest::createBinary() {
	Stream data;
	var channelCount = 0;
	
	#pragma region Sequences
	PArray sequences;
	var masterSequence = NEW_(Sequence);
	masterSequence->writeHeader();
	var time = 0;
	// create first frame
	{
		masterSequence->writeDelta(0);
		sequences.push(masterSequence);
		var si = 1;
		PArray modules;
		modules.push(NEW_(BassDevice, synthAdapter_, player_));
		modules.push(NEW_(SynthDevice, synthAdapter_, player_));

		for (var mi = 0; mi < modules.length(); mi++) {
			var dev = (Device*)modules.get(mi);
			var ext = playerExt_->getDeviceExtension(dev);
			var seq = ext->createDefaultSequence();
			if (seq) {
				sequences.push(seq);
				masterSequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(si)->writeByte(si)->writeByte(si)->writeByte(2);
				var frameList = FrameList::fromSequence(seq, ext);
				var ti = ((Frame*)frameList->get(frameList->length() - 1))->time;
				if (ti > time) time = ti;
				si++;
				channelCount++;
				DEL_(frameList);
			}
			DEL_(ext);
			DEL_(dev);
		}
		//...
		masterSequence->writeCommand(PlayerCommands::CmdEOF);
	}
	//create last frame
	{
		masterSequence->writeDelta(2 * time);
		masterSequence->writeCommand(PlayerCommands::CmdEOS);

	}
	#pragma endregion

	var initData = NEW_(Stream);
	initData->writeFloat(80.0f);
	// number of channels
	initData->writeByte(channelCount);
	
	#pragma region Adapters
	var adapterList = NEW_(Stream);
	// 2 adapters
	adapterList->writeByte(2);
	// 1st adapter: PlayerAdapter
	adapterList->writeByte(masterDevice_->adapter()->getInfo()->id);
	// no init data
	// 0 devices
	adapterList->writeByte(0);

	// 2nd adapter: SynthAdapter
	adapterList->writeByte(synthAdapter_->getInfo()->id);
	// adapter init data: sampling rate
	adapterList->writeWord(48000);
	// devices
	adapterList->writeByte(3);
	// bass: type, datablock id of preset bank, preset id, voice count
	adapterList->writeByte(SynthDevices::DeviceBass)->writeByte(0xff)->writeByte(2)->writeByte(1);
	// synth: type, datablock id of preset bank, preset id, voice count
	adapterList->writeByte(SynthDevices::DeviceSynth)->writeByte(0xff)->writeByte(0)->writeByte(6);
	// mixer: type, datablock id of preset bank, preset id, channel count
	adapterList->writeByte(SynthDevices::DeviceMixer)->writeByte(0xff)->writeByte(0)->writeByte(2);
	// channel1: input id, stage count(, stage1.input, ...)
	adapterList->writeByte(1)->writeByte(0);
	// channel2: input id, stage count(, stage2.input, ...)
	adapterList->writeByte(2)->writeByte(0);
	#pragma endregion
	
	#pragma region Create Stream
	var sequenceList = NEW_(Stream);
	sequenceList->writeByte(sequences.length());
	for (var si = 0; si < sequences.length(); si++) {
		var seq = (Sequence*)sequences.get(si);
		sequenceList->writeWord((word)seq->length());
	}
	for (var si = 0; si < sequences.length(); si++) {
		var seq = (Sequence*)sequences.get(si);
		sequenceList->writeStream(seq);
	}
	sequences.apply([](COLLECTION_ARGUMENTS) { DEL_((Sequence*)value); return value; });
	
	// header
	// 3 data blocks
	data.writeWord(3);
	data.writeDword(initData->length());
	data.writeDword(adapterList->length());
	data.writeDword(sequenceList->length());
	
	data.writeStream(initData);
	DEL_(initData);
	data.writeStream(adapterList);
	DEL_(adapterList);
	data.writeStream(sequenceList);
	DEL_(sequenceList);
	#pragma endregion
	
	return data.extract();
}

void SynthTest::runAll(int& totalPassed, int& totalFailed) {
	//// bass
	//deviceExts()->push(playerExt_->getDeviceExtension((Device*)NEW_(BassDevice, synthAdapter_, player_)));
	//// mixer
	//deviceExts()->push(playerExt_->getDeviceExtension(NEW_(MixerDevice, synthAdapter_, player_)));

	var bin = createBinary();
	player_->initialize(&bin);
	Key key;
	var mixerDev = (MixerDevice*)player_->devices().search(SynthDevices::DeviceMixer, key, [](COLLECTION_ARGUMENTS) { return ((Device*)value)->type() - key.i; });
	mixer_ = mixerDev->mixer();
	
	// create controls
	char lbl[16];
	int x = 10, y = 10;
	for (var di = 1; di < player_->devices().length(); di++) {
		var dev = (Device*)player_->devices().get(di);
		var ext = playerExt_->getDeviceExtension(dev);
		var ctrl = DeviceCtrl::getDeviceControl(ext);
		if (ctrl != NULL) {
			controls()->push(ctrl);
			sprintf_s(lbl, 16, "Ctrl%2d", di);
			ctrl->create(area(), lbl);
			ctrl->move(x, y);
			y += ctrl->rect().bottom + 4;
		}
	}

	mixerDev->setInput(MxCh1amp, 0.6f);
	mixerDev->setInput(MxCh1pan, 108);
	mixerDev->setInput(MxCh1gain, 180);

	mixerDev->setInput(MxCh2amp, 0.4f);
	mixerDev->setInput(MxCh2pan, 148);
	mixerDev->setInput(MxCh2gain, 180);

	player_->useThread();
	player_->masterChannel()->isEndless(true);
	SoundPlayer::start(samplingRate, 2, Mixer8x4::fillSoundBuffer, mixer_);

	task_ = NEW_(SynthTestTask, this);
	task_->run();
}
#pragma endregion