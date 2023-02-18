#include "synth/ui/synth-app.h"
#include "base/string.h"
#include "base/stream.h"
#include "soundlib/src/soundplayer.h"
#include "player/src/player-lib.h"
#include "player/src/player-adapter.h"
#include "synth/src/device/synth-adapter.h"
#include "synth/src/device/synth-device.h"
#include "synth/src/device/bass-device.h"
#include "synth/src/device/mixer-device.h"

NS_FW_WIN_USE

float samplingRate = 48000.0f;

Stream* bassSoundBank_ = NULL;
PlayerAdapter playerAdapter_;
SynthAdapter synthAdapter_;
PlayerDevice* masterDevice_;
Player* player_;
Mixer8x4* mixer_;

Stream* getBassSoundBank() {
    if (bassSoundBank_ == NULL) {
        int count = 0;
        var bass = NEW_(Stream);
        {
            bass->writeByte(28);
            bass->writeByte(BassCtrlId::bAmAdsrAmp)->writeFloat(1.0f);
            bass->writeByte(BassCtrlId::bAmAdsrAtk)->writeByte(12);
            bass->writeByte(BassCtrlId::bAmAdsrDec)->writeByte(60);
            bass->writeByte(BassCtrlId::bAmAdsrSus)->writeByte(160);
            bass->writeByte(BassCtrlId::bAmAdsrRel)->writeByte(100);         // 5

            bass->writeByte(BassCtrlId::bPmAdsrAmp)->writeFloat(0.0f);
            bass->writeByte(BassCtrlId::bPmAdsrAtk)->writeByte(20);
            bass->writeByte(BassCtrlId::bPmAdsrDec)->writeByte(144);
            bass->writeByte(BassCtrlId::bPmAdsrSus)->writeByte(128);
            bass->writeByte(BassCtrlId::bPmAdsrRel)->writeByte(100);       // 5

            bass->writeByte(BassCtrlId::bFtAdsrAmp)->writeFloat(0.2f);
            bass->writeByte(BassCtrlId::bFtAdsrAtk)->writeByte(2);
            bass->writeByte(BassCtrlId::bFtAdsrDec)->writeByte(60);
            bass->writeByte(BassCtrlId::bFtAdsrSus)->writeByte(40);
            bass->writeByte(BassCtrlId::bFtAdsrRel)->writeByte(80);       // 5

            bass->writeByte(BassCtrlId::bOsc1Amp)->writeByte(120);
            bass->writeByte(BassCtrlId::bOsc1Fre)->writeFloat(0.0f);
            bass->writeByte(BassCtrlId::bOsc1Tune)->writeByte(0);
            bass->writeByte(BassCtrlId::bOsc1Psw)->writeByte(120);
            bass->writeByte(BassCtrlId::bOsc1Wave)->writeByte(WfSaw);  // 5

            bass->writeByte(BassCtrlId::bOsc2Amp)->writeByte(60);
            bass->writeByte(BassCtrlId::bOsc2Fre)->writeFloat(0.4f);
            bass->writeByte(BassCtrlId::bOsc2Tune)->writeByte(12);
            bass->writeByte(BassCtrlId::bOsc2Psw)->writeByte(100);
            bass->writeByte(BassCtrlId::bOsc2Wave)->writeByte(WfPulse); // 5

            bass->writeByte(BassCtrlId::bFlt1Cut)->writeByte(10);
            bass->writeByte(BassCtrlId::bFlt1Res)->writeByte(80);
            bass->writeByte(BassCtrlId::bFlt1Mode)->writeByte(FmLowPass);  // 3
            count++;
        };

        var bass2 = NEW_(Stream);
        {
            bass2->writeByte(28);
            bass2->writeByte(BassCtrlId::bAmAdsrAmp)->writeFloat(1.0f);
            bass2->writeByte(BassCtrlId::bAmAdsrAtk)->writeByte(250);
            bass2->writeByte(BassCtrlId::bAmAdsrDec)->writeByte(240);
            bass2->writeByte(BassCtrlId::bAmAdsrSus)->writeByte(220);
            bass2->writeByte(BassCtrlId::bAmAdsrRel)->writeByte(240);         // 5

            bass2->writeByte(BassCtrlId::bPmAdsrAmp)->writeFloat(0.2f);
            bass2->writeByte(BassCtrlId::bPmAdsrAtk)->writeByte(120);
            bass2->writeByte(BassCtrlId::bPmAdsrDec)->writeByte(244);
            bass2->writeByte(BassCtrlId::bPmAdsrSus)->writeByte(88);
            bass2->writeByte(BassCtrlId::bPmAdsrRel)->writeByte(200);       // 5

            bass2->writeByte(BassCtrlId::bFtAdsrAmp)->writeFloat(0.2f);
            bass2->writeByte(BassCtrlId::bFtAdsrAtk)->writeByte(250);
            bass2->writeByte(BassCtrlId::bFtAdsrDec)->writeByte(200);
            bass2->writeByte(BassCtrlId::bFtAdsrSus)->writeByte(120);
            bass2->writeByte(BassCtrlId::bFtAdsrRel)->writeByte(240);       // 5

            bass2->writeByte(BassCtrlId::bOsc1Amp)->writeByte(100);
            bass2->writeByte(BassCtrlId::bOsc1Fre)->writeFloat(0.0f);
            bass2->writeByte(BassCtrlId::bOsc1Tune)->writeByte(0);
            bass2->writeByte(BassCtrlId::bOsc1Psw)->writeByte(120);
            bass2->writeByte(BassCtrlId::bOsc1Wave)->writeByte(WfSaw | WfPulse | WfSinus);  // 5

            bass2->writeByte(BassCtrlId::bOsc2Amp)->writeByte(20);
            bass2->writeByte(BassCtrlId::bOsc2Fre)->writeFloat(3487.04f);
            bass2->writeByte(BassCtrlId::bOsc2Tune)->writeByte(0);
            bass2->writeByte(BassCtrlId::bOsc2Psw)->writeByte(120);
            bass2->writeByte(BassCtrlId::bOsc2Wave)->writeByte(WfNoise); // 5

            bass2->writeByte(BassCtrlId::bFlt1Cut)->writeByte(4);
            bass2->writeByte(BassCtrlId::bFlt1Res)->writeByte(100);
            bass2->writeByte(BassCtrlId::bFlt1Mode)->writeByte(FmLowPass);  // 3
            count++;
        };

        word offset = 1 + count * 16;
        bassSoundBank_ = NEW_(Stream, 4096);
        bassSoundBank_->writeByte(count);
        bassSoundBank_->writeString("bass.........");
        bassSoundBank_->writeWord(offset); offset += (word)bass->length();
        bassSoundBank_->writeString("bass2........");
        bassSoundBank_->writeWord(offset); offset += (word)bass2->length();

        bassSoundBank_->writeStream(bass);
        bassSoundBank_->writeStream(bass2);
        DEL_(bass);
        DEL_(bass2);
    }
    return bassSoundBank_;
}

Stream* createBinaryData() {
    var data = NEW_(Stream);
    PlayerDevice playerDevice(NULL, &playerAdapter_);
    SynthDevice synthDevice(&synthAdapter_);
    BassDevice bassDevice(&synthAdapter_);

    var initData = NEW_(Stream);
    initData->writeFloat(32.0f);
    // 1 channels
    initData->writeByte(1);

#pragma region Sequences
    var steps = 64;
    //#region MASTER sequence - 12
    // k kbh kbhc kbhcm kbhcm kbhm kbhm
    var sequence1 = NEW_(Sequence, &playerDevice);
    {
        sequence1->writeHeader();

        sequence1->writeDelta(0);
        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(1)->writeByte(1);
        sequence1->writeEOF();

        sequence1->writeDelta(steps);
        sequence1->writeEOS();
    }
    //#region Sequence #1 - 64st "bass"
    var sequence2 = NEW_(Sequence, &bassDevice);
    {
        sequence2->writeHeader();
        sequence2->writeDelta(0);
        sequence2->writeCommand(CmdSetNote)->writeByte(pD1)->writeByte(128);
        sequence2->writeEOF();
        sequence2->writeDelta(4);
        sequence2->writeCommand(CmdSetNote)->writeByte(pD1)->writeByte(0);
        sequence2->writeEOF();

        sequence2->writeDelta(4);
        sequence2->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(128);
        sequence2->writeEOF();
        sequence2->writeDelta(4);
        sequence2->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(0);
        sequence2->writeEOF();

        sequence2->writeDelta(4);
        sequence2->writeCommand(CmdSetNote)->writeByte(pF1)->writeByte(128);
        sequence2->writeEOF();
        sequence2->writeDelta(4);
        sequence2->writeCommand(CmdSetNote)->writeByte(pF1)->writeByte(0);
        sequence2->writeEOF();

        sequence2->writeDelta(4);
        sequence2->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(128);
        sequence2->writeEOF();
        sequence2->writeDelta(4);
        sequence2->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(0);
        sequence2->writeEOF();

        sequence2->writeDelta(4);
        sequence2->writeCommand(CmdSetNote)->writeByte(pC1)->writeByte(128);
        sequence2->writeEOF();
        sequence2->writeDelta(4);
        sequence2->writeCommand(CmdSetNote)->writeByte(pC1)->writeByte(0);
        sequence2->writeEOF();

        sequence2->writeDelta(4);
        sequence2->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(128);
        sequence2->writeEOF();
        sequence2->writeDelta(4);
        sequence2->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(0);
        sequence2->writeEOF();

        sequence2->writeDelta(4);
        sequence2->writeCommand(CmdSetNote)->writeByte(pG1)->writeByte(128);
        sequence2->writeEOF();
        sequence2->writeDelta(4);
        sequence2->writeCommand(CmdSetNote)->writeByte(pG1)->writeByte(0);
        sequence2->writeEOF();

        sequence2->writeDelta(4);
        sequence2->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(128);
        sequence2->writeEOF();
        sequence2->writeDelta(4);
        sequence2->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(0);
        sequence2->writeEOF();

        sequence2->writeDelta(4);
        sequence2->writeEOS();
    }

#pragma endregion

    var adapterList = NEW_(Stream);
    // 2 adapters
    adapterList->writeByte(2);
    // 1st adapter: PlayerAdapter
    {
        adapterList->writeByte(playerAdapter_.getInfo()->id);
        // no init data
        // no devices
        adapterList->writeByte(0);
    }
    // 2nd adapter: SynthAdapter
    {
        adapterList->writeByte(synthAdapter_.getInfo()->id);
        adapterList->writeWord(48000);
        // 4 devices
        adapterList->writeByte(4);
        adapterList->writeByte(SynthDevices::DeviceBass);  //dev #1: bass
        adapterList->writeByte(1);  // voice count
        adapterList->writeByte(0);  // data block id for sound bank 
        adapterList->writeByte(0);  // program

        //adapterList->writeByte(SynthDevices::DeviceDrums);  //dev #1: drums

        adapterList->writeByte(SynthDevices::DeviceClip);  //dev #2
        adapterList->writeFloat(2.1f);      // amp
        adapterList->writeByte(180);        // lvl
        adapterList->writeByte( 80);        // cut
        adapterList->writeByte(160);        // res
        adapterList->writeByte(FmLowPass || FmBandPass /*|| FmHighPass */);    // mode

        adapterList->writeByte(SynthDevices::DeviceDelay);  //dev #3
        adapterList->writeByte(160);        // feedbackL
        adapterList->writeFloat(120.0f);    // delayL
        adapterList->writeByte(140);        // feedbackR
        adapterList->writeFloat(240.0f);    // delayR
        adapterList->writeByte(180);        // mixL
        adapterList->writeByte(160);        // mixR

        adapterList->writeByte(SynthDevices::DeviceMixer);  //dev #4
        adapterList->writeByte(1);

        // channel #1
        adapterList->writeByte(120);    // gain#2 - bass
        adapterList->writeByte( 50);    // pan#2
        adapterList->writeByte(200);    // amp#2
        adapterList->writeByte(  2);    // stage count
        adapterList->writeByte( 40);    // stage1 gain
        adapterList->writeByte( 20);    // stage2 gain

        adapterList->writeByte(1);      // channel1 input: bass
        adapterList->writeByte(2);      // channel1.stage1
        adapterList->writeByte(3);      // channel1.stage2
    }

#pragma region CreateStream
    var sequenceList = NEW_(Stream);
    sequenceList->writeByte(2);
    sequenceList->writeWord((word)sequence1->length());
    sequenceList->writeWord((word)sequence2->length());
    sequenceList->writeStream(sequence1);
    sequenceList->writeStream(sequence2);
    DEL_(sequence1);
    DEL_(sequence2);

    var bassSoundBank = getBassSoundBank();

    // header
    // 3 data blocks
    data->writeWord(4);
    data->writeDword(initData->length());
    data->writeDword(adapterList->length());
    data->writeDword(sequenceList->length());
    data->writeDword(bassSoundBank->length());

    data->writeStream(initData);
    DEL_(initData);
    data->writeStream(adapterList);
    DEL_(adapterList);
    data->writeStream(sequenceList);
    DEL_(sequenceList);
    data->writeStream(bassSoundBank);
    DEL_(bassSoundBank);
#pragma endregion

    return data;

}

void loadBinary() {
    var bin = createBinaryData();
    Player::addAdapter(NEW_(SynthAdapter));
    var data = bin->extract();
    DEL_(bin);
    masterDevice_ = PlayerDevice::create(&data);
    player_ = masterDevice_->player();
    player_->assignChannel(0, (Sequence*)player_->sequences().get(0), 0, 0);
    Key ix = 0;
    mixer_ = (Mixer8x4*)((Device*)player_->devices().search(NULL, ix,
        [](COLLECTION_ARGUMENTS) { return ((Device*)value)->type() == SynthDevices::DeviceMixer ? 0 : 1; }))->object();
}


SynthApp::SynthApp() {
	bassCtrl_ = NULL;
}

SynthApp::~SynthApp() {
	DEL_(bassCtrl_->module());
	DEL_(bassCtrl_);
}

void SynthApp::create(WndClass wndClass, LONG style, DWORD exStyle) {
	Window::create(wndClass, NULL, "TestApp", style, exStyle);
}

LRESULT SynthApp::onCreate() {
	LRESULT result = 0;
	SYSFN(result, SetWindowPos(hWnd_, NULL, 0, 0, 800, 600, SWP_NOMOVE));

    loadBinary();

    var bass = (BassDevice*)player_->devices().get(1);
	bassCtrl_ = NEW_(BassCtrl, (Bass*)bass->object());
	bassCtrl_->create(this, "Bass");

	//SYSPR(SetWindowPos(potCtrl1_.hWnd(), NULL, 0, 0, 32, 32, SWP_SHOWWINDOW));
	return 0;
}

int SynthApp::main(NS_FW_BASE::Map* args) {
    MSG msg;
    int isExiting = 0;

    player_->masterChannel()->isEndless(true);
    player_->useThread();
    SoundPlayer::start((int)samplingRate, 2, Mixer8x4::fillSoundBuffer, mixer_);
    player_->start();
    while (!isExiting) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            //LOG("msg: %08x\n", msg.message);
            if (msg.message != WM_QUIT) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else {
                isExiting = true;
            }
        }
        update();
    }
    player_->stop();
    SoundPlayer::stop();
    DEL_(masterDevice_);
    Player::cleanUp();
    return (int)msg.wParam;
}


WinApp* createApplication(HINSTANCE hInstance, Map* args) {
	// Load menu
	HMENU hMenu = NULL;
	var wndClass = Window::registerClass((char*)"SynthApp", hInstance);
	var app = NEW_(SynthApp);
	app->create(wndClass);
	return app;
}
