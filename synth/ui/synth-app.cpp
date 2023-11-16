#include "synth/ui/synth-app.h"
#include "base/string.h"
#include "base/stream.h"
#include "soundlib/src/soundplayer.h"
#include "synth/src/device/synth-device.h"
#include "synth/src/device/synth2-device.h"
#include "synth/src/device/bass-device.h"
#include "synth/src/device/drums-device.h"
#include "synth/src/device/distort-device.h"
#include "synth/src/device/stereo-delay-device.h"
#include "synth/src/xmloader.h"

NS_FW_WIN_USE
using namespace SYNTH_APP;

float samplingRate = 48000.0f;

//Stream* SynthApp::createBinaryData() {
//    var data = NEW_(Stream);
//    PlayerDevice playerDevice(NULL, &playerAdapter_);
//    SynthDevice synthDevice(&synthAdapter_);
//    BassDevice bassDevice(&synthAdapter_);
//    DrumsDevice drumsDevice(&synthAdapter_);
//
//    var initData = NEW_(Stream);
//    initData->writeFloat(32.0f);
//    // 4 channels
//    initData->writeByte(4);
//
//    #pragma region Sequences
//    var steps = 256;
//    //#region MASTER sequence - 12
//    // k kbh kbhc kbhcm kbhcm kbhm kbhm
//    var sequence1 = NEW_(Sequence, &playerDevice);
//    {
//        sequence1->writeHeader();
//
//        sequence1->writeDelta(0);
//        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(1)->writeByte(8);
//        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(2)->writeByte(2)->writeByte(3)->writeByte(8);
//        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(3)->writeByte(3)->writeByte(2)->writeByte(1);
//        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(4)->writeByte(5)->writeByte(4)->writeByte(4);
//        sequence1->writeEOF();
//
//        sequence1->writeDelta(steps);
//        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(3)->writeByte(4)->writeByte(2)->writeByte(1);
//        sequence1->writeEOF();
//
//        sequence1->writeDelta(steps);
//        sequence1->writeEOS();
//    }
//    //#region Sequence #1 - 64st "bass"
//    var sequence2 = NEW_(Sequence, &bassDevice);
//    {
//        sequence2->writeHeader();
//        sequence2->writeDelta(0);
//        sequence2->writeCommand(CmdSetNote)->writeByte(pD1)->writeByte(128);
//        sequence2->writeEOF();
//        sequence2->writeDelta(4);
//        sequence2->writeCommand(CmdSetNote)->writeByte(pD1)->writeByte(0);
//        sequence2->writeEOF();
//
//        sequence2->writeDelta(4);
//        sequence2->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(128);
//        sequence2->writeEOF();
//        sequence2->writeDelta(4);
//        sequence2->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(0);
//        sequence2->writeEOF();
//
//        sequence2->writeDelta(4);
//        sequence2->writeCommand(CmdSetNote)->writeByte(pF1)->writeByte(128);
//        sequence2->writeEOF();
//        sequence2->writeDelta(4);
//        sequence2->writeCommand(CmdSetNote)->writeByte(pF1)->writeByte(0);
//        sequence2->writeEOF();
//
//        sequence2->writeDelta(4);
//        sequence2->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(128);
//        sequence2->writeEOF();
//        sequence2->writeDelta(4);
//        sequence2->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(0);
//        sequence2->writeEOF();
//
//        sequence2->writeDelta(4);
//        sequence2->writeCommand(CmdSetNote)->writeByte(pC1)->writeByte(128);
//        sequence2->writeEOF();
//        sequence2->writeDelta(4);
//        sequence2->writeCommand(CmdSetNote)->writeByte(pC1)->writeByte(0);
//        sequence2->writeEOF();
//
//        sequence2->writeDelta(4);
//        sequence2->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(128);
//        sequence2->writeEOF();
//        sequence2->writeDelta(4);
//        sequence2->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(0);
//        sequence2->writeEOF();
//
//        sequence2->writeDelta(4);
//        sequence2->writeCommand(CmdSetNote)->writeByte(pG1)->writeByte(128);
//        sequence2->writeEOF();
//        sequence2->writeDelta(4);
//        sequence2->writeCommand(CmdSetNote)->writeByte(pG1)->writeByte(0);
//        sequence2->writeEOF();
//
//        sequence2->writeDelta(4);
//        sequence2->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(128);
//        sequence2->writeEOF();
//        sequence2->writeDelta(4);
//        sequence2->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(0);
//        sequence2->writeEOF();
//
//        sequence2->writeDelta(4);
//        sequence2->writeEOS();
//    }
//    //#region Sequence #3 - 32st "drums"
//    var sequence3 = NEW_(Sequence, &drumsDevice);
//    {
//        sequence3->writeHeader();
//
//        sequence3->writeDelta(0);
//        sequence3->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(208);
//        sequence3->writeCommand(CmdSetNote)->writeByte(drBD)->writeByte(200);
//        sequence3->writeEOF();
//
//        sequence3->writeDelta(8);
//        sequence3->writeCommand(CmdSetNote)->writeByte(drOH)->writeByte(208);
//        sequence3->writeEOF();
//
//        sequence3->writeDelta(8);
//        sequence3->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(208);
//        sequence3->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte(120);
//        sequence3->writeEOF();
//
//        sequence3->writeDelta(8);
//        sequence3->writeCommand(CmdSetNote)->writeByte(drOH)->writeByte(208);
//        sequence3->writeEOF();
//
//        sequence3->writeDelta(4);
//        sequence3->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(208);
//        sequence3->writeCommand(CmdSetNote)->writeByte(drBD)->writeByte(200);
//        sequence3->writeEOF();
//        sequence3->writeDelta(3);
//        sequence3->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(198);
//        sequence3->writeEOF();
//        sequence3->writeDelta(5);
//        sequence3->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(148);
//        sequence3->writeEOF();
//
//        sequence3->writeDelta(4);
//        sequence3->writeCommand(CmdSetNote)->writeByte(drOH)->writeByte(208);
//        sequence3->writeCommand(CmdSetNote)->writeByte(drBD)->writeByte(160);
//        sequence3->writeEOF();
//
//        sequence3->writeDelta(8);
//        sequence3->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(208);
//        sequence3->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte(120);
//        sequence3->writeEOF();
//
//        sequence3->writeDelta(8);
//        sequence3->writeCommand(CmdSetNote)->writeByte(drOH)->writeByte(208);
//        sequence3->writeEOF();
//
//        sequence3->writeDelta(8);
//        sequence3->writeEOS();
//    }
//    //#region Sequence #4 - 256st "chords"
//    var sequence4 = NEW_(Sequence, &synthDevice);
//    {
//        sequence4->writeHeader();
//
//        sequence4->writeDelta(0);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(100);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pF4)->writeByte(110);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pA4)->writeByte(90);
//        sequence4->writeEOF();
//
//        sequence4->writeDelta(16);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(0);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pF4)->writeByte(0);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pA4)->writeByte(0);
//        sequence4->writeEOF();
//
//        sequence4->writeDelta(16);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(100);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pE4)->writeByte(110);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pG4)->writeByte(90);
//        sequence4->writeEOF();
//
//        sequence4->writeDelta(16);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(0);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pE4)->writeByte(0);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pG4)->writeByte(0);
//        sequence4->writeEOF();
//
//        sequence4->writeDelta(16);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(100);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(110);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pF4)->writeByte(90);
//        sequence4->writeEOF();
//
//        sequence4->writeDelta(16);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(0);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(0);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pF4)->writeByte(0);
//        sequence4->writeEOF();
//
//        sequence4->writeDelta(16);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(100);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(110);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pE4)->writeByte(90);
//        sequence4->writeEOF();
//
//        sequence4->writeDelta(16);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(0);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(0);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pE4)->writeByte(0);
//        sequence4->writeEOF();
//
//        sequence4->writeDelta(16);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pF3)->writeByte(100);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(110);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(90);
//        sequence4->writeEOF();
//
//        sequence4->writeDelta(16);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pF3)->writeByte(0);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(0);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(0);
//        sequence4->writeEOF();
//
//        sequence4->writeDelta(16);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pE3)->writeByte(100);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(110);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(90);
//        sequence4->writeEOF();
//
//        sequence4->writeDelta(16);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pE3)->writeByte(0);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(0);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(0);
//        sequence4->writeEOF();
//
//        sequence4->writeDelta(16);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pF3)->writeByte(100);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(110);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(90);
//        sequence4->writeEOF();
//
//        sequence4->writeDelta(16);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pF3)->writeByte(0);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(0);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(0);
//        sequence4->writeEOF();
//
//        sequence4->writeDelta(16);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(100);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pH3)->writeByte(110);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(90);
//        sequence4->writeEOF();
//
//        sequence4->writeDelta(16);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(0);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pH3)->writeByte(0);
//        sequence4->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(0);
//        sequence4->writeEOF();
//
//        sequence4->writeDelta(16);
//        sequence4->writeEOS();
//    }
//    //#region Sequence #2 - 128st "chords"
//    // A C
//    var sequence5 = NEW_(Sequence, &synthDevice);
//    {
//        sequence5->writeHeader();
//
//        sequence5->writeDelta(0);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(100);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pF4)->writeByte(110);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pA4)->writeByte(90);
//        sequence5->writeEOF();
//
//        sequence5->writeDelta(16);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(0);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pF4)->writeByte(0);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pA4)->writeByte(0);
//        sequence5->writeEOF();
//
//        sequence5->writeDelta(16);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pE4)->writeByte(100);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pG4)->writeByte(110);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(90);
//        sequence5->writeEOF();
//
//        sequence5->writeDelta(16);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pE4)->writeByte(0);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pG4)->writeByte(0);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(0);
//        sequence5->writeEOF();
//
//        sequence5->writeDelta(16);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(100);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pF4)->writeByte(110);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pA4)->writeByte(90);
//        sequence5->writeEOF();
//
//        sequence5->writeDelta(16);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(0);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pF4)->writeByte(0);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pA4)->writeByte(0);
//        sequence5->writeEOF();
//
//        sequence5->writeDelta(16);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(100);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pE4)->writeByte(110);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pG4)->writeByte(90);
//        sequence5->writeEOF();
//
//        sequence5->writeDelta(16);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(0);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pE4)->writeByte(0);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pG4)->writeByte(0);
//        sequence5->writeEOF();
//
//        sequence5->writeDelta(16);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(100);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(110);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pF4)->writeByte(90);
//        sequence5->writeEOF();
//
//        sequence5->writeDelta(16);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(0);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(0);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pF4)->writeByte(0);
//        sequence5->writeEOF();
//
//        sequence5->writeDelta(16);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(100);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pE4)->writeByte(110);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pG4)->writeByte(90);
//        sequence5->writeEOF();
//
//        sequence5->writeDelta(16);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(0);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pE4)->writeByte(0);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pG4)->writeByte(0);
//        sequence5->writeEOF();
//
//        sequence5->writeDelta(16);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(100);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(110);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pF4)->writeByte(90);
//        sequence5->writeEOF();
//
//        sequence5->writeDelta(16);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(0);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(0);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pF4)->writeByte(0);
//        sequence5->writeEOF();
//
//        sequence5->writeDelta(16);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(100);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(110);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pE4)->writeByte(90);
//        sequence5->writeEOF();
//
//        sequence5->writeDelta(16);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(0);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(0);
//        sequence5->writeCommand(CmdSetNote)->writeByte(pE4)->writeByte(0);
//        sequence5->writeEOF();
//
//        sequence5->writeDelta(16);
//        sequence5->writeEOS();
//    }
//
//    var sequence6 = NEW_(Sequence, &synthDevice);
//    {
//        sequence6->writeHeader();
//
//        sequence6->writeDelta(0);
//        sequence6->writeCommand(CmdSetUint8)->writeByte(distCut)->writeByte(10);
//        sequence6->writeEOF();
//
//        sequence6->writeDelta(16);
//        sequence6->writeCommand(CmdSetUint8)->writeByte(distCut)->writeByte(20);
//        sequence6->writeEOF();
//
//        sequence6->writeDelta(16);
//        sequence6->writeCommand(CmdSetUint8)->writeByte(distCut)->writeByte(30);
//        sequence6->writeEOF();
//
//        sequence6->writeDelta(16);
//        sequence6->writeCommand(CmdSetUint8)->writeByte(distCut)->writeByte(40);
//        sequence6->writeEOF();
//
//        sequence6->writeDelta(16);
//        sequence6->writeCommand(CmdSetUint8)->writeByte(distCut)->writeByte(50);
//        sequence6->writeEOF();
//
//        sequence6->writeDelta(16);
//        sequence6->writeCommand(CmdSetUint8)->writeByte(distCut)->writeByte(60);
//        sequence6->writeEOF();
//
//        sequence6->writeDelta(16);
//        sequence6->writeCommand(CmdSetUint8)->writeByte(distCut)->writeByte(70);
//        sequence6->writeEOF();
//
//        sequence6->writeDelta(16);
//        sequence6->writeCommand(CmdSetUint8)->writeByte(distCut)->writeByte(80);
//        sequence6->writeEOF();
//
//        sequence6->writeDelta(16);
//        sequence6->writeCommand(CmdSetUint8)->writeByte(distCut)->writeByte(90);
//        sequence6->writeEOF();
//
//        sequence6->writeDelta(16);
//        sequence6->writeCommand(CmdSetUint8)->writeByte(distCut)->writeByte(80);
//        sequence6->writeEOF();
//
//        sequence6->writeDelta(16);
//        sequence6->writeCommand(CmdSetUint8)->writeByte(distCut)->writeByte(70);
//        sequence6->writeEOF();
//
//        sequence6->writeDelta(16);
//        sequence6->writeCommand(CmdSetUint8)->writeByte(distCut)->writeByte(60);
//        sequence6->writeEOF();
//
//        sequence6->writeDelta(16);
//        sequence6->writeCommand(CmdSetUint8)->writeByte(distCut)->writeByte(50);
//        sequence6->writeEOF();
//
//        sequence6->writeDelta(16);
//        sequence6->writeCommand(CmdSetUint8)->writeByte(distCut)->writeByte(40);
//        sequence6->writeEOF();
//
//        sequence6->writeDelta(16);
//        sequence6->writeCommand(CmdSetUint8)->writeByte(distCut)->writeByte(30);
//        sequence6->writeEOF();
//
//        sequence6->writeDelta(16);
//        sequence6->writeCommand(CmdSetUint8)->writeByte(distCut)->writeByte(20);
//        sequence6->writeEOF();
//
//        sequence6->writeDelta(16);
//        sequence6->writeEOS();
//    }
//    #pragma endregion
//
//    var adapterList = NEW_(Stream);
//    // 2 adapters
//    adapterList->writeByte(2);
//    // 1st adapter: PlayerAdapter
//    {
//        adapterList->writeByte(playerAdapter_.getInfo()->id);
//        // no init data
//        // no devices
//        adapterList->writeByte(0);
//    }
//    // 2nd adapter: SynthAdapter
//    {
//        adapterList->writeByte(synthAdapter_.getInfo()->id);
//        adapterList->writeWord(48000);
//        // 7 devices
//        adapterList->writeByte(7);
//        adapterList->writeByte(SynthDevices::DeviceBass);  //dev #1: bass
//        adapterList->writeByte(1);  // voice count
//        adapterList->writeByte(255);// default soundbank
//        adapterList->writeByte(0);  // program
//        adapterList->writeByte(SynthDevices::DeviceSynth);  //dev #2: synth
//        adapterList->writeByte(6);  // voice count
//        adapterList->writeByte(255);// default soundbank
//        adapterList->writeByte(0);  // program
//        adapterList->writeByte(SynthDevices::DeviceDrums);  //dev #3: drums
//        adapterList->writeByte(255);// default soundbank
//        adapterList->writeByte(0)->writeByte(1)->writeByte(2)->writeByte(3);    // programs
//        adapterList->writeByte(0)->writeByte(0)->writeByte(0)->writeByte(0);
//
//        adapterList->writeByte(SynthDevices::DeviceDistort);  //dev #4
//        adapterList->writeByte(255);    // default soundbank
//        adapterList->writeByte(0);      // program id
//
//        adapterList->writeByte(SynthDevices::DeviceStereoDelay);  //dev #5
//        adapterList->writeByte(255);    // default soundbank
//        adapterList->writeByte(0);      // program id
//
//        adapterList->writeByte(SynthDevices::DeviceStereoDelay);  //dev #6
//        adapterList->writeByte(255);    // default soundbank
//        adapterList->writeByte(2);      // program id
//
//        adapterList->writeByte(SynthDevices::DeviceMixer);      //dev #6
//        // 3 channels
//        adapterList->writeByte(3);
//        // adapterList->writeByte(255);// default soundbank
//        // adapterList->writeByte(0)    // program id
//
//        // channel #1
//        adapterList->writeByte(100);    // amp#1
//        adapterList->writeByte(127);    // pan#1
//        adapterList->writeByte( 20);    // gain#1 - bass
//        adapterList->writeByte(  2);    // stage count
//        adapterList->writeByte( 20);    // stage1 gain
//        adapterList->writeByte(100);    // stage2 gain
//
//        // channel #2
//        adapterList->writeByte( 60);    // amp#3
//        adapterList->writeByte( 90);    // pan#3
//        adapterList->writeByte(200);    // gain#3 - synth
//        adapterList->writeByte(  1);    // stage count
//        adapterList->writeByte(100);    // stage1 gain
//
//        // channel #3
//        adapterList->writeByte( 60);    // amp#2
//        adapterList->writeByte(127);    // pan#2
//        adapterList->writeByte(200);    // gain#2 - drums
//        adapterList->writeByte(  0);    // stage count
//
//        adapterList->writeByte(1);      // channel1 input: bass
//        adapterList->writeByte(4);      // channel1.stage1
//        adapterList->writeByte(5);      // channel1.stage2
//        adapterList->writeByte(2);      // channel2 input: synth
//        adapterList->writeByte(6);      // channel2.stage1
//        adapterList->writeByte(3);      // channel3 input: drums
//    }
//
//    #pragma region CreateStream
//    var sequenceList = NEW_(Stream);
//    sequenceList->writeByte(6);
//    sequenceList->writeWord((word)sequence1->length());
//    sequenceList->writeWord((word)sequence2->length());
//    sequenceList->writeWord((word)sequence3->length());
//    sequenceList->writeWord((word)sequence4->length());
//    sequenceList->writeWord((word)sequence5->length());
//    sequenceList->writeWord((word)sequence6->length());
//    sequenceList->writeStream(sequence1);
//    sequenceList->writeStream(sequence2);
//    sequenceList->writeStream(sequence3);
//    sequenceList->writeStream(sequence4);
//    sequenceList->writeStream(sequence5);
//    sequenceList->writeStream(sequence6);
//    DEL_(sequence1);
//    DEL_(sequence2);
//    DEL_(sequence3);
//    DEL_(sequence4);
//    DEL_(sequence5);
//    DEL_(sequence6);
//
//    // header
//    // 5 data blocks
//    data->writeWord(3);
//    data->writeDword(initData->length());
//    data->writeDword(adapterList->length());
//    data->writeDword(sequenceList->length());
//
//    data->writeStream(initData);
//    DEL_(initData);
//    data->writeStream(adapterList);
//    DEL_(adapterList);
//    data->writeStream(sequenceList);
//    DEL_(sequenceList);
//    #pragma endregion
//
//    return data;
//
//}
//
//Stream* SynthApp::createDefault() {
//    var data = NEW_(Stream);
//
//    PlayerDevice playerDevice(NULL, &playerAdapter_);
//    ModuleDevice* devices[] = {
//        NEW_(BassDevice, &synthAdapter_),
//        NEW_(SynthDevice, &synthAdapter_),
//        NEW_(Synth2Device, &synthAdapter_),
//        NEW_(DrumsDevice, &synthAdapter_)
//    };
//    int count = arraysize(devices);
//    ModuleCtrl* controls[] = {
//        NEW_(BassCtrl, devices[2]->module()),
//        NEW_(SynthCtrl, devices[0]->module()),
//        NEW_(Synth2Ctrl, devices[1]->module()),
//        NEW_(DrumsCtrl, devices[3]->module())
//    };
//
//    var initData = NEW_(Stream);
//    initData->writeFloat(4*24.0f);
//    // 5 channels
//    initData->writeByte(5);
//
//    #pragma region Sequences
//    PArray sequences;
//    var sequenceList = NEW_(Stream);
//    // create master sequence
//    var masterSeq = NEW_(Sequence, &playerDevice);
//    {
//        masterSeq->writeHeader();
//        masterSeq->writeDelta(0);
//        for (var ci = 1; ci <= count; ci++) {
//            masterSeq->writeCommand(PlayerCommands::CmdAssign)->writeByte(ci)->writeByte(ci)->writeByte(ci)->writeByte(1);
//        }
//        masterSeq->writeEOF();
//        masterSeq->writeDelta(128);
//        masterSeq->writeEOS();
//
//        sequences.push(masterSeq);
//    };
//    // create default sequence for each controller/device
//    for (var ci = 0; ci < count; ci++) {
//        sequences.push(controls[ci]->createDefaultSequence(devices[ci]));
//    }
//    sequenceList->writeByte(sequences.length());
//    for (var si = 0; si < sequences.length(); si++) {
//        sequenceList->writeWord((word)((Sequence*)sequences.get(si))->length());
//    }
//    for (var si = 0; si < sequences.length(); si++) {
//        var seq = (Sequence*)sequences.get(si);
//        sequenceList->writeStream(seq);
//        DEL_(seq);
//    }
//    #pragma endregion
//
//    #pragma region Adapters
//    var adapterList = NEW_(Stream);
//    adapterList->writeByte(2);
//    // 1st adapter: PlayerAdapter
//    {
//        adapterList->writeByte(playerAdapter_.getInfo()->id);
//        // no init data
//        // no devices
//        adapterList->writeByte(0);
//    }
//    // 2nd adapter: SynthAdapter
//    {
//        var deviceId = 1;       // #0: player device
//        adapterList->writeByte(synthAdapter_.getInfo()->id);
//        adapterList->writeWord(48000);
//        // device count
//        adapterList->writeByte(count * 3 + 1);        // synth + 2 effects + mixer
//        // write devices
//        adapterList->writeByte(SynthDevices::DeviceBass);   //dev #1: bass
//        adapterList->writeByte(1);  // voice count
//        adapterList->writeByte(255);// default soundbank
//        adapterList->writeByte(0);  // program
//        deviceId++;
//        adapterList->writeByte(SynthDevices::DeviceSynth);  //dev #2: synth
//        adapterList->writeByte(6);  // voice count
//        adapterList->writeByte(255);// default soundbank
//        adapterList->writeByte(0);  // program
//        deviceId++;
//        adapterList->writeByte(SynthDevices::DeviceSynth2); //dev #3: synth2
//        adapterList->writeByte(6);  // voice count
//        adapterList->writeByte(255);// default soundbank
//        adapterList->writeByte(0);  // program
//        deviceId++;
//        adapterList->writeByte(SynthDevices::DeviceDrums);  //dev #4: drums
//        adapterList->writeByte(255);// default soundbank
//        adapterList->writeByte(0)->writeByte(1)->writeByte(2)->writeByte(3);    // programs
//        adapterList->writeByte(0)->writeByte(0)->writeByte(0)->writeByte(0);
//        deviceId++;
//        // add effects for each synth
//        for (var di = 0; di < count; di++) {
//            adapterList->writeByte(SynthDevices::DeviceDistort);
//            adapterList->writeByte(255);    // default soundbank
//            adapterList->writeByte(0);      // program id
//            adapterList->writeByte(SynthDevices::DeviceStereoDelay);
//            adapterList->writeByte(255);    // default soundbank
//            adapterList->writeByte(0);      // program id
//        }
//        // add mixer
//        adapterList->writeByte(SynthDevices::DeviceMixer);
//        adapterList->writeByte(count);
//        // adapterList->writeByte(255);// default soundbank
//        // adapterList->writeByte(0)    // program id
//        // channel settings
//        for (var ci = 0; ci < count; ci++) {
//            adapterList->writeByte(ci == 2 ? 0x80 : 0x00);    // amp
//            var pan = (byte)(ci * 255.0f / count);
//            adapterList->writeByte(128);    // pan
//            adapterList->writeByte(80);     // gain
//            adapterList->writeByte(2);      // stage count
//            adapterList->writeByte(20);     // stage1 gain
//            adapterList->writeByte(80);     // stage2 gain
//        }
//
//        for (var di = 0; di <= count; di++) {
//            adapterList->writeByte(1 + di);     // channel input
//            adapterList->writeByte(deviceId++); // channel.stage1 input
//            adapterList->writeByte(deviceId++); // channel.stage2 input
//        }
//    }
//    #pragma endregion
//
//    #pragma region CreateStream
//    // 3 data blocks
//    data->writeWord(3);
//    data->writeDword(initData->length());
//    data->writeDword(adapterList->length());
//    data->writeDword(sequenceList->length());
//
//    data->writeStream(initData);
//    DEL_(initData);
//    data->writeStream(adapterList);
//    DEL_(adapterList);
//    data->writeStream(sequenceList);
//    DEL_(sequenceList);
//    #pragma endregion
//
//    for (var di = 0; di < count; di++) {
//        DEL_(devices[di]);
//    }
//
//    return data;
//}
//
//void SynthApp::loadBinary(bool useDefault) {
//    var bin = useDefault ? createDefault() : createBinaryData();
//    var data = bin->extract();
//    DEL_(bin);
//    masterDevice_ = PlayerDevice::create(&data);
//    player_ = masterDevice_->player();
//    //player_->assignChannel(0, (Sequence*)player_->sequences().get(0), 0, 0);
//    Key ix = 0;
//    mixer_ = (Mixer8x4*)((Device*)player_->devices().search(NULL, ix,
//        [](COLLECTION_ARGUMENTS) { return ((Device*)value)->type() == SynthDevices::DeviceMixer ? 0 : 1; }))->object();
//    loadSoundbanks();
//}
//
//void SynthApp::loadModule(const char* path) {
//    masterDevice_ = PlayerDevice::create();
//    player_ = masterDevice_->player();
//    loadSoundbanks();
//    var xmLoader = NEW_(XmLoader, masterDevice_);
//    xmLoader->load(path, soundbanks_);
//    mixer_ = (Mixer8x4*)((MixerDevice*)player_->addDevice(&synthAdapter_, SynthDevices::DeviceMixer))->module();
//    mixer_->channelCount(player_->devices().length() - 2);
//    for (var di = 0; di < mixer_->channelCount(); di++) {
//        var channel = mixer_->getChannel(di);
//        mixer_->connectInput(channel, ((ModuleDevice*)player_->devices().get(di + 1))->module());
//        channel->controls->amp.value.f = 0.3f;
//        channel->controls->gain.value.f = 0.8f;
//        channel->controls->pan.value.f = 0.5f;
//        channel->stageCount = 0;
//    }
//    DEL_(xmLoader);
//}
//
//void SynthApp::loadSoundbanks() {
//    soundbanks_ = ModuleDevice::loadSoundbanks("soundbanks.bin", &synthAdapter_);
//    //soundbanks_->hasRefKey(false);
//    //byte *bytes = NULL, *end = NULL;
//    //// try to read file
//    //var bytesRead = File::read("soundbanks.bin", &bytes);
//    //if (bytesRead != -1) {
//    //    var bptr = bytes;
//    //    end = bytes + bytesRead;
//    //    while (bptr < end) {
//    //        // read device type
//    //        var deviceType = READ(bptr, byte);
//    //        // create dummy device
//    //        var dev = (ModuleDevice*)synthAdapter_.createDevice(deviceType);
//    //        var sb = dev->module()->createSoundbank();
//    //        sb->initializeFromStream(bptr);
//    //        soundbanks_->put(deviceType, sb);
//    //        DEL_(dev);
//    //    }
//    //}
//    //FREE(bytes);
//
//    // the missing soundbanks are copied from device's default
//    for (var i = 0; i < player_->devices().length(); i++) {
//        var dev = (ModuleDevice*)player_->devices().get(i);
//        var mdl = dev->module();
//        if (dev->adapter()->getInfo()->id == synthAdapter_.getInfo()->id) {
//            Key key(dev->type());
//            var defaultSoundbank = mdl->getDefaultSoundbank();
//            var sb = (Soundbank*)soundbanks_->get(key);
//            if (defaultSoundbank != NULL && sb == NULL) {
//                // duplicate the default soundbank
//                sb = mdl->createSoundbank();
//                sb->copy(defaultSoundbank, 0, -1);
//                soundbanks_->put(key, sb);
//            }
//            if (sb != NULL) {
//                mdl->setSoundbank(sb);
//                //mdl->setProgram(mdl->program());
//            }
//        }
//    }
//}
//
//void SynthApp::saveSoundbanks() {
//    Stream stream;
//    for (var i = 0; i < soundbanks_->size(); i++) {
//        stream.writeByte(*(int*)soundbanks_->keys()->get(i));
//        var sb = (Soundbank*)soundbanks_->values()->get(i);
//        sb->writeToStream(&stream);
//        DEL_(sb);
//    }
//    stream.writeBytesToFile("soundbanks.bin");
//}

SynthApp::SynthApp() {
    activePlayer_ = NULL;
    uiManager_ = NEW_(UiManager, this);
    fileManager_ = NEW_(FileManager, this);
    creator_ = NULL;

    activeSong_ = NULL;
}

SynthApp::~SynthApp() {
    songs_.apply(
        [](COLLECTION_ARGUMENTS) {
            DEL_(value);
            return value;
        });

    DEL_(uiManager_);
    DEL_(fileManager_);
}

void SynthApp::create(WndClass wndClass) {
	Window::create(wndClass, NULL, "SynthApp", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN);
}

LRESULT SynthApp::onCreate() {
	LRESULT result = 0;
	SYSFN(result, SetWindowPos(hWnd_, NULL, 0, 0, 1208, 940, SWP_NOMOVE));
    Player::addAdapter(NEW_(SynthAdapter));

    // create song
    activeSong_ = NEW_(Song, fileManager_->loadDefault(), this);
    songs_.push(activeSong_);
    activePlayer_ = activeSong_->player();

    // start playback
    activePlayer_->masterChannel()->isEndless(true);
    activePlayer_->useThread();
    SoundPlayer::start((int)samplingRate, 2, Mixer8x4::fillSoundBuffer, activeSong_->mixerDevice()->mixer());
    activePlayer_->start();

    //loadBinary(true);
    //loadModule("D:\\Audio\\Dalok\\mod\\1m.xm");
    //int left = 0, top = 0;
    //int synthStackHeight = 0;
    //for (var i = 0; i < player_->devices().length(); i++) {
    //    ModuleCtrl* ctrl = NULL;
    //    var dev = (ModuleDevice*)player_->devices().get(i);
    //    var mdl = dev->module();
    //    if (dev->adapter()->getInfo()->id == synthAdapter_.getInfo()->id) {
    //        switch (dev->type()) {
    //        case DeviceBass:
    //            ctrl = NEW_(BassCtrl, mdl);
    //            ctrl->create(this, "Bass");
    //            break;
    //        case DeviceSynth:
    //            ctrl = NEW_(SynthCtrl, mdl);
    //            ctrl->create(this, "Synth");
    //            break;
    //        case DeviceSynth2:
    //            ctrl = NEW_(Synth2Ctrl, mdl);
    //            ctrl->create(this, "Synth2");
    //            break;
    //        case DeviceDrums:
    //            ctrl = NEW_(DrumsCtrl, mdl);
    //            ctrl->create(this, "Drums");
    //            synthStackHeight += ctrl->rect().bottom;
    //            break;
    //        case DeviceDistort:
    //            ctrl = NEW_(DistortCtrl, mdl);
    //            ctrl->create(this, "Distort");
    //            break;
    //        case DeviceStereoDelay:
    //            ctrl = NEW_(StereoDelayCtrl, mdl);
    //            ctrl->create(this, "StereoDelay");
    //            break;
    //        case DeviceMixer:
    //            ctrl = NEW_(MixerCtrl, mdl);
    //            ctrl->create(this, "Mixer");
    //            break;
    //        }
    //        var sb = mdl->soundbank();
    //        if (sb) {
    //            //ctrl->updateSoundbank();
    //            //ModuleCtrl::soundbankSetter(ctrl, sb);
    //            //ModuleCtrl::programSetter(ctrl, mdl->program());
    //        }
    //    }
    //    if (ctrl != NULL) {
    //        ctrls_.push(ctrl);
    //    }
    //}

    return 0;
}

LRESULT SynthApp::onSize(RECT& rect, WPARAM state) {
    //if (updateLayout()) {
    //    // erase background
    //    SYSPR(InvalidateRect(hWnd_, NULL, true));
    //}
    return 1;
}

//int SynthApp::updateLayout() {
//    int hasChanged = 0;
//    var wi = rect_.right, he = rect_.bottom;
//    char name[16];
//    int x = 0, y = 0, dw = wi, rowHe = 0;
//    for (var i = 0; i < ctrls_.length(); i++) {
//        var mdl = (ModuleCtrl*)ctrls_.get(i);
//        if (dw < mdl->rect().right) {
//            x = 0;
//            dw = wi;
//            y += rowHe;
//            rowHe = 0;
//        }
//        dw -= mdl->rect().right;
//        SYSPR(GetWindowText(mdl->hWnd(), name, 16));
//        //LOG("%s: %d - %d, %d - %d\n", name, mdl->offset().x, x, mdl->offset().y, y);
//        var isMoved = mdl->offset().x != x || mdl->offset().y != y;
//        if (isMoved) {
//            mdl->move(x, y);
//            SYSPR(InvalidateRect(mdl->hWnd(), NULL, true));
//            hasChanged++;
//        }
//        if (rowHe < mdl->rect().bottom) rowHe = mdl->rect().bottom;
//        x += mdl->rect().right;
//    }
//
//    return hasChanged;
//}

int SynthApp::main(NS_FW_BASE::Map* args) {
    MSG msg;
    int isExiting = 0;

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

    activePlayer_->stop();
    SoundPlayer::stop();

    //saveSoundbanks();

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