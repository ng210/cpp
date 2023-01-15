#include <math.h>
#include "test.h"
#include "console/consoleapp.h"
#include "soundlib/src/soundplayer.h"
#include "soundlib/src/wavfmt.h"
#include "player/src/player-lib.h"
#include "player/src/player-adapter.h"
//#include "synth/src/module/synth-controls.h"
#include "synth/src/device/synth-device.h"
#include "synth/src/device/bass-device.h"
//#include "synth/src/mixer-controls.h"
#include "synth/src/device/mixer-device.h"
#include "synth/src/device/distort-device.h"
#include "synth/src/module/distort.h"
//#include "synth/src/mix.h"
#include "synth/src/xmloader.h"

NS_FW_BASE_USE
using namespace SYNTH;
using namespace PLAYER;

#define SAMPLING_RATE 48000
#define LAST_TICK 128

class SynthTest : public Test {
    //Player* player_;
    //Device* device_;

    static PlayerAdapter playerAdapter_;
    static SynthAdapter synthAdapter_;

    //Synth* synth_ = NULL;

    Stream* createSoundBank();
    Stream* createBassSoundBank();
    Stream* createBinaryData();
    void createTestSequence(PlayerDevice* device);
    //Stream* createSynthInitData();
public:
    static float buffer_[4096];
    float samplingRate;

    SynthTest();
    void testFilterSound();
    void testGenerateSoundSimple();
    void testGenerateSoundMixer();
    void testCreatePlayerWithDevices();
    void testLoadBinary();
    void testCreateBinary();
    void testSaveToWave();
    void testLoadXm();
};

float SynthTest::buffer_[4096];

void simpleSynthCallback(short* buffer, int sampleCount, void* context) {
    //((Synth*)context)->run(buffer, 0, sampleCount);
    for (var i = 0; i < sampleCount; i++) {
        SynthTest::buffer_[i] = 0.0f;
    }
    var synth = (Synth*)context;
    synth->run(0, sampleCount);
    for (var i = 0; i < sampleCount; i++) {
        buffer[i] = (short)(synth->getOutput(0)[i] * 32768.0f);
    }
}

PlayerAdapter SynthTest::playerAdapter_;
SynthAdapter SynthTest::synthAdapter_;

SynthTest::SynthTest() {
    //device_ = NULL;
    //player_ = NULL;
    samplingRate = SAMPLING_RATE;
}

void SynthTest::createTestSequence(PlayerDevice* device) {
    var player = device->player();
    var sequence1 = NEW_(Sequence, device);
    { // sequence #1
        sequence1->writeByte(PlayerDevices::DevicePlayer);
        // Frame #1 - 8
        sequence1->writeDelta(0);
        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(3)->writeByte(0);
        sequence1->writeEOF();
        // Frame #2 - 3
        sequence1->writeDelta(LAST_TICK);
        sequence1->writeEOS();
        player->addSequence(sequence1);
    }
    // 2nd sequence
    var sequence2 = NEW_(Sequence, NULL);
    {
        sequence2->writeByte(SynthDevices::DeviceSynth);
        // Frame #1 - 11
        sequence2->writeDelta(0);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pC4)->writeByte(180);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pDs4)->writeByte(80);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pG4)->writeByte(160);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pC2)->writeByte(120);
        sequence2->writeEOF();
        // Frame #2 - 11
        sequence2->writeDelta(24);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pC4)->writeByte(0);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pDs4)->writeByte(0);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pG4)->writeByte(0);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pC2)->writeByte(0);
        sequence2->writeEOF();
        // Frame #3 - 11
        sequence2->writeDelta(40);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pC4)->writeByte(180);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pDs4)->writeByte(80);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pG4)->writeByte(160);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pC2)->writeByte(120);
        sequence2->writeEOF();
        // Frame #4 - 11
        sequence2->writeDelta(24);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pC4)->writeByte(0);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pDs4)->writeByte(0);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pG4)->writeByte(0);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pC2)->writeByte(0);
        sequence2->writeEOF();
        // Frame #5 - 11
        sequence2->writeDelta(40);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pAs3)->writeByte(180);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pC4)->writeByte(80);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pF4)->writeByte(160);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pAs1)->writeByte(120);
        sequence2->writeEOF();
        // Frame #6 - 11
        sequence2->writeDelta(80);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pAs3)->writeByte(0);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pC4)->writeByte(0);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pF4)->writeByte(0);
        sequence2->writeByte(SynthCommands::CmdSetNote)->writeByte(SynthPitch::pAs1)->writeByte(0);
        sequence2->writeEOF();

        // Frame #3 - 3
        sequence2->writeDelta(48);
        sequence2->writeEOS();
        player->addSequence(sequence2);
    }
}

//Stream* SynthTest::createSynthInitData() {
//    var data = NEW_(Stream);
//    //data->write
//    return data;
//}
//
//Stream* SynthTest::createBinary() {
//    var data = NEW_(Stream);
//    createSequences();
//    var sequence1 = (Sequence*)playerDevice_->sequences().get(0);
//    var sequence2 = (Sequence*)playerDevice_->sequences().get(1);
//
//    #pragma region Datablocks
//    var deviceTypeList = NEW_(Stream);
//    deviceTypeList->writeByte(1);                   // 1 device type
//    deviceTypeList->writeByte(1 + 1 + 1 + 1 + 2);   // length of init data block
//    deviceTypeList->writeByte(DeviceSynth);         // 01 type
//    deviceTypeList->writeByte(6);                   // 02 voice count
//    deviceTypeList->writeByte(1);                   // 03 data block id of sound bank
//    deviceTypeList->writeByte(3);                   // 04 default program
//    deviceTypeList->writeWord(SamplingRate);
//    var soundBank = createSoundBank();
//
//    data->writeWord(3);                             // data block count
//    data->writeDword(deviceTypeList->length());     // length of device type list block
//    data->writeDword(1 + 2 * 2 + sequence1->length() + sequence2->length());    // length of sequence list
//    //data->writeDword(synthInit->length());    // length of data block 1
//    data->writeDword(soundBank->length());          // length of data block 2
//    data->writeFloat(25.0f);                        // fps = 25.0
//    data->writeByte(1);                         // 1 channel
//    // device type list
//    data->writeStream(deviceTypeList);
//    DEL_(deviceTypeList);
//    // sequence list
//    data->writeByte(2);                             // 2 sequences
//    data->writeWord((word)sequence1->length());
//    data->writeWord((word)sequence2->length());
//    // 1st sequence
//    data->writeStream(sequence1);
//    data->writeStream(sequence2);
//    DEL_(sequence1);
//    DEL_(sequence2);
//    // data block #1: init data for SynthDevice
//    // data block #2: sound bank
//    //data->writeStream(synthInit);
//    data->writeStream(soundBank);
//    DEL_(soundBank);
//    #pragma endregion
//
//    return data;
//
//}

Stream* SynthTest::createBassSoundBank() {
    int count = 0;
    var bass = NEW_(Stream);
    {
        count++;
        bass->writeByte(28);
        bass->writeByte(BassCtrlId::bAmEnvAmp)->writeFloat(1.0f);
        bass->writeByte(BassCtrlId::bAmEnvAtk)->writeByte(22);
        bass->writeByte(BassCtrlId::bAmEnvDec)->writeByte(58);
        bass->writeByte(BassCtrlId::bAmEnvSus)->writeByte(40);
        bass->writeByte(BassCtrlId::bAmEnvRel)->writeByte(122);       // 5

        bass->writeByte(BassCtrlId::bPmEnvAmp)->writeFloat(0.2f);
        bass->writeByte(BassCtrlId::bPmEnvAtk)->writeByte(40);
        bass->writeByte(BassCtrlId::bPmEnvDec)->writeByte(144);
        bass->writeByte(BassCtrlId::bPmEnvSus)->writeByte(128);
        bass->writeByte(BassCtrlId::bPmEnvRel)->writeByte(200);       // 5

        bass->writeByte(BassCtrlId::bFtEnvAmp)->writeFloat(0.8f);
        bass->writeByte(BassCtrlId::bFtEnvAtk)->writeByte(22);
        bass->writeByte(BassCtrlId::bFtEnvDec)->writeByte(62);
        bass->writeByte(BassCtrlId::bFtEnvSus)->writeByte(10);
        bass->writeByte(BassCtrlId::bFtEnvRel)->writeByte(32);       // 5

        bass->writeByte(BassCtrlId::bOsc1Amp)->writeByte(160);
        bass->writeByte(BassCtrlId::bOsc1Fre)->writeFloat(0.0f);
        bass->writeByte(BassCtrlId::bOsc1Tune)->writeByte(0);
        bass->writeByte(BassCtrlId::bOsc1Psw)->writeByte(120);
        bass->writeByte(BassCtrlId::bOsc1Wave)->writeByte(WfPulse | WfSaw);  // 5

        bass->writeByte(BassCtrlId::bOsc2Amp)->writeByte(120);
        bass->writeByte(BassCtrlId::bOsc2Fre)->writeFloat(1.2f);
        bass->writeByte(BassCtrlId::bOsc2Tune)->writeByte(12);
        bass->writeByte(BassCtrlId::bOsc2Psw)->writeByte(100);
        bass->writeByte(BassCtrlId::bOsc2Wave)->writeByte(WfPulse); // 5

        bass->writeByte(BassCtrlId::bFlt1Cut)->writeByte(20);
        bass->writeByte(BassCtrlId::bFlt1Res)->writeByte(120);
        bass->writeByte(BassCtrlId::bFlt1Mode)->writeByte(FmLowPass | FmBandPass);  // 3
    };

    word offset = 1 + count * 16;
    Stream* s = NEW_(Stream, 1024);
    s->writeByte(count);
    s->writeString("bass.........");
    s->writeWord(offset); offset += (word)bass->length();

    s->writeStream(bass);
    DEL_(bass);
    return s;
}

Stream* SynthTest::createSoundBank() {
    int count = 0;
    var bass = NEW_(Stream);
    {
        count++;
        bass->writeByte(32);
        bass->writeByte(SynthCtrlId::amEnvAmp)->writeFloat(1.0f);
        //bass->writeByte(SynthCtrlId::amEnvDc)->writeFloat(0.0f);
        bass->writeByte(SynthCtrlId::amEnvAtk)->writeByte(22);
        bass->writeByte(SynthCtrlId::amEnvDec)->writeByte(68);
        bass->writeByte(SynthCtrlId::amEnvSus)->writeByte(80);
        bass->writeByte(SynthCtrlId::amEnvRel)->writeByte(122);       // 5

        //bass->writeByte(SynthCtrlId::fmEnvAmp)->writeFloat(0.0f);
        ////bass->writeByte(SynthCtrlId::fmEnvDc)->writeFloat(0.0f);
        //bass->writeByte(SynthCtrlId::fmEnvAtk)->writeByte(0);
        //bass->writeByte(SynthCtrlId::fmEnvDec)->writeByte(0);
        //bass->writeByte(SynthCtrlId::fmEnvSus)->writeByte(0);
        //bass->writeByte(SynthCtrlId::fmEnvRel)->writeByte(0);        // 5

        bass->writeByte(SynthCtrlId::pmEnvAmp)->writeFloat(0.2f);
        //bass->writeByte(SynthCtrlId::pmEnvDc)->writeFloat(0.6f);
        bass->writeByte(SynthCtrlId::pmEnvAtk)->writeByte(40);
        bass->writeByte(SynthCtrlId::pmEnvDec)->writeByte(144);
        bass->writeByte(SynthCtrlId::pmEnvSus)->writeByte(128);
        bass->writeByte(SynthCtrlId::pmEnvRel)->writeByte(200);       // 5

        bass->writeByte(SynthCtrlId::ftEnvAmp)->writeFloat(0.3f);
        bass->writeByte(SynthCtrlId::ftEnvAtk)->writeByte(62);
        bass->writeByte(SynthCtrlId::ftEnvDec)->writeByte(122);
        bass->writeByte(SynthCtrlId::ftEnvSus)->writeByte(80);
        bass->writeByte(SynthCtrlId::ftEnvRel)->writeByte(222);       // 5

        bass->writeByte(SynthCtrlId::amLfoAmp)->writeFloat(0.0f);
        bass->writeByte(SynthCtrlId::amLfoFre)->writeFloat(2.0f);
        bass->writeByte(SynthCtrlId::fmLfoAmp)->writeFloat(0.0f);
        bass->writeByte(SynthCtrlId::fmLfoFre)->writeFloat(3.8f);    // 4

        bass->writeByte(SynthCtrlId::osc1Amp)->writeByte(160);
        bass->writeByte(SynthCtrlId::osc1Fre)->writeFloat(0.0f);
        bass->writeByte(SynthCtrlId::osc1Tune)->writeByte(0);
        bass->writeByte(SynthCtrlId::osc1Psw)->writeByte(120);
        bass->writeByte(SynthCtrlId::osc1Wave)->writeByte(WfPulse);  // 5

        bass->writeByte(SynthCtrlId::osc2Amp)->writeByte(120);
        bass->writeByte(SynthCtrlId::osc2Fre)->writeFloat(0.2f);
        bass->writeByte(SynthCtrlId::osc2Tune)->writeByte(12);
        bass->writeByte(SynthCtrlId::osc2Psw)->writeByte(100);
        bass->writeByte(SynthCtrlId::osc2Wave)->writeByte(WfPulse);    // 5

        bass->writeByte(SynthCtrlId::flt1Cut)->writeByte(80);
        bass->writeByte(SynthCtrlId::flt1Res)->writeByte(160);
        bass->writeByte(SynthCtrlId::flt1Mode)->writeByte(FmLowPass | FmBandPass);  // 3
    }
    var kick = NEW_(Stream);
    {
        count++;
        kick->writeByte(32);
        kick->writeByte(SynthCtrlId::amEnvAmp)->writeFloat(1.0f);
        kick->writeByte(SynthCtrlId::amEnvAtk)->writeByte(2);
        kick->writeByte(SynthCtrlId::amEnvDec)->writeByte(82);
        kick->writeByte(SynthCtrlId::amEnvSus)->writeByte(140);
        kick->writeByte(SynthCtrlId::amEnvRel)->writeByte(26);       // 5

        kick->writeByte(SynthCtrlId::fmEnvAmp)->writeFloat(120.0f);
        kick->writeByte(SynthCtrlId::fmEnvAtk)->writeByte(1);
        kick->writeByte(SynthCtrlId::fmEnvDec)->writeByte(42);
        kick->writeByte(SynthCtrlId::fmEnvSus)->writeByte(4);
        kick->writeByte(SynthCtrlId::fmEnvRel)->writeByte(40);       // 5

        //kick->writeByte(SynthCtrlId::pmEnvAmp)->writeFloat(0.2f);
        //kick->writeByte(SynthCtrlId::pmEnvAtk)->writeByte(0);
        //kick->writeByte(SynthCtrlId::pmEnvDec)->writeByte(54);
        //kick->writeByte(SynthCtrlId::pmEnvSus)->writeByte(108);
        //kick->writeByte(SynthCtrlId::pmEnvRel)->writeByte(40);

        kick->writeByte(SynthCtrlId::ftEnvAmp)->writeFloat(1.0f);
        kick->writeByte(SynthCtrlId::ftEnvAtk)->writeByte(1);
        kick->writeByte(SynthCtrlId::ftEnvDec)->writeByte(8);
        kick->writeByte(SynthCtrlId::ftEnvSus)->writeByte(1);
        kick->writeByte(SynthCtrlId::ftEnvRel)->writeByte(2);        // 5

        kick->writeByte(SynthCtrlId::amLfoAmp)->writeFloat(0.3f);
        kick->writeByte(SynthCtrlId::amLfoFre)->writeFloat(22.0f);
        kick->writeByte(SynthCtrlId::fmLfoAmp)->writeFloat(1.1f);
        kick->writeByte(SynthCtrlId::fmLfoFre)->writeFloat(0.3f);    // 4

        kick->writeByte(SynthCtrlId::osc1Amp)->writeByte(230);
        kick->writeByte(SynthCtrlId::osc1Fre)->writeFloat(0.0f);
        kick->writeByte(SynthCtrlId::osc1Tune)->writeByte(0);
        kick->writeByte(SynthCtrlId::osc1Psw)->writeByte(0);
        kick->writeByte(SynthCtrlId::osc1Wave)->writeByte(WfSinus);  // 5

        kick->writeByte(SynthCtrlId::osc2Amp)->writeByte(100);
        kick->writeByte(SynthCtrlId::osc2Fre)->writeFloat(9210.0f);
        kick->writeByte(SynthCtrlId::osc2Tune)->writeByte(0);
        kick->writeByte(SynthCtrlId::osc2Psw)->writeByte(12);
        kick->writeByte(SynthCtrlId::osc2Wave)->writeByte(WfNoise);  // 5

        kick->writeByte(SynthCtrlId::flt1Cut)->writeByte(0);
        kick->writeByte(SynthCtrlId::flt1Res)->writeByte(60);
        kick->writeByte(SynthCtrlId::flt1Mode)->writeByte(FmLowPass);    // 3
    }
    var snare = NEW_(Stream);
    {
        count++;
        snare->writeByte(32);
        snare->writeByte(SynthCtrlId::amEnvAmp)->writeFloat(1.0f);
        snare->writeByte(SynthCtrlId::amEnvAtk)->writeByte(1);
        snare->writeByte(SynthCtrlId::amEnvDec)->writeByte(30);
        snare->writeByte(SynthCtrlId::amEnvSus)->writeByte(80);
        snare->writeByte(SynthCtrlId::amEnvRel)->writeByte(60);       // 5

        snare->writeByte(SynthCtrlId::fmEnvAmp)->writeFloat(120.0f);
        snare->writeByte(SynthCtrlId::fmEnvAtk)->writeByte(1);
        snare->writeByte(SynthCtrlId::fmEnvDec)->writeByte(10);
        snare->writeByte(SynthCtrlId::fmEnvSus)->writeByte(2);
        snare->writeByte(SynthCtrlId::fmEnvRel)->writeByte(120);       // 5

        snare->writeByte(SynthCtrlId::ftEnvAmp)->writeFloat(1.0f);
        snare->writeByte(SynthCtrlId::ftEnvAtk)->writeByte(2);
        snare->writeByte(SynthCtrlId::ftEnvDec)->writeByte(40);
        snare->writeByte(SynthCtrlId::ftEnvSus)->writeByte(100);
        snare->writeByte(SynthCtrlId::ftEnvRel)->writeByte(20);        // 5

        snare->writeByte(SynthCtrlId::amLfoAmp)->writeFloat(0.2f);
        snare->writeByte(SynthCtrlId::amLfoFre)->writeFloat(33.0f);
        snare->writeByte(SynthCtrlId::fmLfoAmp)->writeFloat(91.1f);
        snare->writeByte(SynthCtrlId::fmLfoFre)->writeFloat(153.3f);    // 4

        snare->writeByte(SynthCtrlId::osc1Amp)->writeByte(230);
        snare->writeByte(SynthCtrlId::osc1Fre)->writeFloat(121.0f);
        snare->writeByte(SynthCtrlId::osc1Tune)->writeByte(0);
        snare->writeByte(SynthCtrlId::osc1Psw)->writeByte(80);
        snare->writeByte(SynthCtrlId::osc1Wave)->writeByte(WfSinus);  // 5

        snare->writeByte(SynthCtrlId::osc2Amp)->writeByte(50);
        snare->writeByte(SynthCtrlId::osc2Fre)->writeFloat(9163.63f);
        snare->writeByte(SynthCtrlId::osc2Tune)->writeByte(0);
        snare->writeByte(SynthCtrlId::osc2Psw)->writeByte(0);
        snare->writeByte(SynthCtrlId::osc2Wave)->writeByte(WfNoise);  // 5

        snare->writeByte(SynthCtrlId::flt1Cut)->writeByte(230);
        snare->writeByte(SynthCtrlId::flt1Res)->writeByte(40);
        snare->writeByte(SynthCtrlId::flt1Mode)->writeByte(FmLowPass | FmBandPass);    // 3
    }
    var hihat = NEW_(Stream);
    {
        count++;
        hihat->writeByte(32);
        hihat->writeByte(SynthCtrlId::amEnvAmp)->writeFloat(1.0f);
        hihat->writeByte(SynthCtrlId::amEnvAtk)->writeByte(1);
        hihat->writeByte(SynthCtrlId::amEnvDec)->writeByte(64);
        hihat->writeByte(SynthCtrlId::amEnvSus)->writeByte(60);
        hihat->writeByte(SynthCtrlId::amEnvRel)->writeByte(20);      // 5

        hihat->writeByte(SynthCtrlId::fmEnvAmp)->writeFloat(21.0f);
        hihat->writeByte(SynthCtrlId::fmEnvAtk)->writeByte(4);
        hihat->writeByte(SynthCtrlId::fmEnvDec)->writeByte(12);
        hihat->writeByte(SynthCtrlId::fmEnvSus)->writeByte(4);
        hihat->writeByte(SynthCtrlId::fmEnvRel)->writeByte(220);     // 5

        hihat->writeByte(SynthCtrlId::ftEnvAmp)->writeFloat(1.0f);
        hihat->writeByte(SynthCtrlId::ftEnvAtk)->writeByte(2);
        hihat->writeByte(SynthCtrlId::ftEnvDec)->writeByte(11);
        hihat->writeByte(SynthCtrlId::ftEnvSus)->writeByte(200);
        hihat->writeByte(SynthCtrlId::ftEnvRel)->writeByte(80);      // 5

        hihat->writeByte(SynthCtrlId::amLfoAmp)->writeFloat(0.0f);
        hihat->writeByte(SynthCtrlId::amLfoFre)->writeFloat(284.0f);
        hihat->writeByte(SynthCtrlId::fmLfoAmp)->writeFloat(0.0f);
        hihat->writeByte(SynthCtrlId::fmLfoFre)->writeFloat(180.3f); // 4

        hihat->writeByte(SynthCtrlId::osc1Amp)->writeByte(160);
        hihat->writeByte(SynthCtrlId::osc1Fre)->writeFloat(286.1f);
        hihat->writeByte(SynthCtrlId::osc1Tune)->writeByte(0);
        hihat->writeByte(SynthCtrlId::osc1Psw)->writeByte(120);
        hihat->writeByte(SynthCtrlId::osc1Wave)->writeByte(WfPulse); // 5

        hihat->writeByte(SynthCtrlId::osc2Amp)->writeByte(120);
        hihat->writeByte(SynthCtrlId::osc2Fre)->writeFloat(521.3f);
        hihat->writeByte(SynthCtrlId::osc2Tune)->writeByte(0);
        hihat->writeByte(SynthCtrlId::osc2Psw)->writeByte(130);
        hihat->writeByte(SynthCtrlId::osc2Wave)->writeByte(WfPulse); // 5

        hihat->writeByte(SynthCtrlId::flt1Cut)->writeByte(240);
        hihat->writeByte(SynthCtrlId::flt1Res)->writeByte(80);
        hihat->writeByte(SynthCtrlId::flt1Mode)->writeByte(FmHighPass);    // 3
    }
    var chords = NEW_(Stream);
    {
        count++;
        chords->writeByte(32);
        chords->writeByte(SynthCtrlId::amEnvAmp)->writeFloat(1.0f);
        //chords->writeByte(SynthCtrlId::amEnvDc)->writeFloat(0.0f);
        chords->writeByte(SynthCtrlId::amEnvAtk)->writeByte(22);
        chords->writeByte(SynthCtrlId::amEnvDec)->writeByte(68);
        chords->writeByte(SynthCtrlId::amEnvSus)->writeByte(140);
        chords->writeByte(SynthCtrlId::amEnvRel)->writeByte(162);       // 5

        //chords->writeByte(SynthCtrlId::fmEnvAmp)->writeFloat(0.0f);
        //chords->writeByte(SynthCtrlId::fmEnvDc)->writeFloat(0.0f);
        //chords->writeByte(SynthCtrlId::fmEnvAtk)->writeByte(0);
        //chords->writeByte(SynthCtrlId::fmEnvDec)->writeByte(0);
        //chords->writeByte(SynthCtrlId::fmEnvSus)->writeByte(0);
        //chords->writeByte(SynthCtrlId::fmEnvRel)->writeByte(0);        // 5

        chords->writeByte(SynthCtrlId::pmEnvAmp)->writeFloat(0.1f);
        //chords->writeByte(SynthCtrlId::pmEnvDc)->writeFloat(0.45f);
        chords->writeByte(SynthCtrlId::pmEnvAtk)->writeByte(2);
        chords->writeByte(SynthCtrlId::pmEnvDec)->writeByte(84);
        chords->writeByte(SynthCtrlId::pmEnvSus)->writeByte(128);
        chords->writeByte(SynthCtrlId::pmEnvRel)->writeByte(40);       // 5

        chords->writeByte(SynthCtrlId::ftEnvAmp)->writeFloat(0.4f);
        chords->writeByte(SynthCtrlId::ftEnvAtk)->writeByte(62);
        chords->writeByte(SynthCtrlId::ftEnvDec)->writeByte(202);
        chords->writeByte(SynthCtrlId::ftEnvSus)->writeByte(120);
        chords->writeByte(SynthCtrlId::ftEnvRel)->writeByte(162);       // 5

        chords->writeByte(SynthCtrlId::amLfoAmp)->writeFloat(0.3f);
        chords->writeByte(SynthCtrlId::amLfoFre)->writeFloat(4.1f);
        chords->writeByte(SynthCtrlId::fmLfoAmp)->writeFloat(6.4f);
        chords->writeByte(SynthCtrlId::fmLfoFre)->writeFloat(4.8f);    // 4

        chords->writeByte(SynthCtrlId::osc1Amp)->writeByte(80);
        chords->writeByte(SynthCtrlId::osc1Fre)->writeFloat(0.0f);
        chords->writeByte(SynthCtrlId::osc1Tune)->writeByte(0);
        chords->writeByte(SynthCtrlId::osc1Psw)->writeByte(120);
        chords->writeByte(SynthCtrlId::osc1Wave)->writeByte(WfSaw);  // 5

        chords->writeByte(SynthCtrlId::osc2Amp)->writeByte(120);
        chords->writeByte(SynthCtrlId::osc2Fre)->writeFloat(1.1f);
        chords->writeByte(SynthCtrlId::osc2Tune)->writeByte(12);
        chords->writeByte(SynthCtrlId::osc2Psw)->writeByte(140);
        chords->writeByte(SynthCtrlId::osc2Wave)->writeByte(WfSaw);    // 5

        chords->writeByte(SynthCtrlId::flt1Cut)->writeByte(20);
        chords->writeByte(SynthCtrlId::flt1Res)->writeByte(60);
        chords->writeByte(SynthCtrlId::flt1Mode)->writeByte(FmLowPass | FmBandPass);  // 3
    }
    var mono = NEW_(Stream);
    {
        count++;
        mono->writeByte(28);
        mono->writeByte(SynthCtrlId::amEnvAmp)->writeFloat(1.0f);
        mono->writeByte(SynthCtrlId::amEnvAtk)->writeByte(2);
        mono->writeByte(SynthCtrlId::amEnvDec)->writeByte(38);
        mono->writeByte(SynthCtrlId::amEnvSus)->writeByte(80);
        mono->writeByte(SynthCtrlId::amEnvRel)->writeByte(42);       // 5

        mono->writeByte(SynthCtrlId::pmEnvAmp)->writeFloat(0.2f);
        mono->writeByte(SynthCtrlId::pmEnvAtk)->writeByte(20);
        mono->writeByte(SynthCtrlId::pmEnvDec)->writeByte(64);
        mono->writeByte(SynthCtrlId::pmEnvSus)->writeByte(128);
        mono->writeByte(SynthCtrlId::pmEnvRel)->writeByte(40);       // 5

        mono->writeByte(SynthCtrlId::ftEnvAmp)->writeFloat(1.0f);
        mono->writeByte(SynthCtrlId::ftEnvAtk)->writeByte(2);
        mono->writeByte(SynthCtrlId::ftEnvDec)->writeByte(32);
        mono->writeByte(SynthCtrlId::ftEnvSus)->writeByte(80);
        mono->writeByte(SynthCtrlId::ftEnvRel)->writeByte(42);       // 5

        mono->writeByte(SynthCtrlId::osc1Amp)->writeByte(160);
        mono->writeByte(SynthCtrlId::osc1Fre)->writeFloat(0.0f);
        mono->writeByte(SynthCtrlId::osc1Tune)->writeByte(0);
        mono->writeByte(SynthCtrlId::osc1Psw)->writeByte(0);
        mono->writeByte(SynthCtrlId::osc1Wave)->writeByte(WfSaw);  // 5

        mono->writeByte(SynthCtrlId::osc2Amp)->writeByte(120);
        mono->writeByte(SynthCtrlId::osc2Fre)->writeFloat(0.1f);
        mono->writeByte(SynthCtrlId::osc2Tune)->writeByte(12);
        mono->writeByte(SynthCtrlId::osc2Psw)->writeByte(10);
        mono->writeByte(SynthCtrlId::osc2Wave)->writeByte(WfSaw);    // 5

        mono->writeByte(SynthCtrlId::flt1Cut)->writeByte(20);
        mono->writeByte(SynthCtrlId::flt1Res)->writeByte(120);
        mono->writeByte(SynthCtrlId::flt1Mode)->writeByte(FmLowPass);  // 3
    }

    word offset = 1 + count * 16;
    Stream* s = NEW_(Stream, 1024);
    s->writeByte(count);
    s->writeString("bassline.....");
    s->writeWord(offset); offset += (word)bass->length();
    s->writeString("kick.........");
    s->writeWord(offset); offset += (word)kick->length();
    s->writeString("snare........");
    s->writeWord(offset); offset += (word)snare->length();
    s->writeString("hihat........");
    s->writeWord(offset); offset += (word)hihat->length();
    s->writeString("chords.......");
    s->writeWord(offset); offset += (word)chords->length();
    s->writeString("mono.........");
    s->writeWord(offset); offset += (word)mono->length();

    s->writeStream(bass);
    s->writeStream(kick);
    s->writeStream(snare);
    s->writeStream(hihat);
    s->writeStream(chords);
    s->writeStream(mono);

    DEL_(bass);
    DEL_(kick);
    DEL_(snare);
    DEL_(hihat);
    DEL_(chords);
    DEL_(mono);

    return s;
}

void simpleSoundCallback(short* buffer, int count, void* p) {
    var elems = (Elem**)p;
    var flt = (Flt*)elems[0];
    var osc1 = (Osc*)elems[1];
    var osc2 = (Osc*)elems[2];
    var osc3 = (Osc*)elems[3];

    float oscParams[] = { 1.0f, 0.0f, 0.0f };
    for (var i = 0; i < count; i++) {
        var smp = osc1->run((float*)&oscParams) + osc2->run((float*)&oscParams) + osc3->run((float*)&oscParams);
        smp = flt->run(smp);
        buffer[i] = (short)(smp * 32768.0f);
    }
}

void SynthTest::testFilterSound() {
    Osc::initialize(&samplingRate);
    Lfo::initialize(&samplingRate);
    Env::initialize(samplingRate);
    Flt::initialize(samplingRate);
    // prepare oscillators
    const int minPole = 1;
    const int maxPoles = 6;
    const float freq = 0.01f * samplingRate - 1.0f;
    const float cut = freq / samplingRate;

    OscCtrls osc1Controls, osc2Controls, osc3Controls;
    Pot note1;
    note1.value.b = pNone;
    Osc osc1, osc2, osc3;
    osc1.assignControls((PotBase*)&osc1Controls);
    osc1.setNoteControl(&note1);
    osc1Controls.fre.value.f = 0.5f * samplingRate - 1.0f;
    osc1Controls.wave.value.b = OscWaveform::WfNoise;

    osc2.assignControls((PotBase*)&osc2Controls);
    osc2.setNoteControl(&note1);
    osc2Controls.fre.value.f = 4.0f * freq;
    osc2Controls.psw.value.f = 0.5f;
    osc2Controls.wave.value.b = OscWaveform::WfPulse;

    osc3.assignControls((PotBase*)&osc3Controls);
    osc3.setNoteControl(&note1);
    osc3Controls.fre.value.f = 0.25f * freq;
    osc3Controls.psw.value.f = 0.5f;
    osc3Controls.wave.value.b = OscWaveform::WfPulse;

    // prepare filters
    FltCtrls fltControls;
    Flt* flts[16];
    Flt::initialize(samplingRate);
    for (var i = minPole; i < maxPoles; i++) {
        var flt = NEW_(Flt, i);
        flts[i] = flt;
        flt->assignControls((PotBase*)&fltControls);
        flt->update(cut);
    }
    fltControls.cut.value.b = 0;
    fltControls.res.value.f = 1.0f;
    fltControls.mod.value.f = 1.0f;
    
    void* fltModes[] = {
        "AllPass", (void*)FmAllPass,
        "LowPass", (void*)FmLowPass,
        "BandPass", (void*)FmBandPass,
        "HighPass", (void*)FmHighPass
    };
    Elem* elems[] = { NULL, &osc1, &osc2, &osc3 };
    for (var m = 0; m < ARRAYSIZE(fltModes);) {
        var txt = fltModes[m++];
        fltControls.mode.value.b = *(byte*)&fltModes[m++];
        for (var i = minPole; i < maxPoles; i++) {
            LOG(" - %d pole %s\n", i+1, txt);
            elems[0] = flts[i];
            SoundPlayer::start((int)samplingRate, 1, simpleSoundCallback, &elems);
            Sleep(600);
            SoundPlayer::stop();
            Sleep(100);
        }
    }

    for (var i = minPole; i < maxPoles; i++) {
        DEL_(flts[i]);
    }
}

void SynthTest::testGenerateSoundSimple() {
    const int note1 = SynthPitch::pD5;
    const int note2 = SynthPitch::pFs4;
    const int note3 = SynthPitch::pA4;
    const int note4 = SynthPitch::pD2;
    //Mdl::createFrequencyTable();
    var synth = NEW_(Synth, &samplingRate, 4);
    byte* sb = NULL;
    var soundBank = createSoundBank();
    synth->soundBank(soundBank->data());
    synth->setProgram(4);

    if (SoundPlayer::start((int)samplingRate, 1, simpleSynthCallback, synth) == 0) {
        for (var i = 0; i < 2; i++) {
            synth->setNote(note1, 124);
            synth->setNote(note2, 84);
            synth->setNote(note3, 114);
            synth->setNote(note4, 74);
            Sleep(2400);
            synth->setNote(note1, 0);
            synth->setNote(note2, 0);
            synth->setNote(note3, 0);
            synth->setNote(note4, 0);
            Sleep(1200);
        }
        Sleep(1000);
        SoundPlayer::stop();
    }

    assert("Should create sound", synth->isActive());

    DEL_(synth);
    DEL_(soundBank);
}

void SynthTest::testGenerateSoundMixer() {
    const int note1 = SynthPitch::pFs3;
    const int note2 = SynthPitch::pA3;
    const int note3 = SynthPitch::pD4;
    const int note4 = SynthPitch::pD1;
    //Mdl::createFrequencyTable();
#pragma region Devices
    var soundBank = createSoundBank();
    var synth = NEW_(Synth, &samplingRate, 1);
    synth->voiceCount(6);
    synth->soundBank(soundBank->data());
    synth->setProgram(4);

    var bassSoundBank = createBassSoundBank();
    var bass = NEW_(Bass, &samplingRate, 1);
    bass->soundBank(bassSoundBank->data());
    bass->setProgram(0);

    var clip1 = NEW_(Distort, &samplingRate);
    var mixer = NEW_(Mixer8x4);
        mixer->channelCount(2);
#pragma endregion

    // channel 1
    mixer->connectInput(mixer->getChannel(0), synth);
    mixer->getControl(MCch1gain)->value.f = 1.0f;
    mixer->getControl(MCch1pan)->value.f = 0.2f;
    mixer->getControl(MCch1amp)->value.f = 0.5f;

    // channel 2
    var ch2 = mixer->getChannel(1);
    mixer->connectInput(ch2, bass);
    mixer->getControl(MCch2gain)->value.f = 0.2f;
    mixer->getControl(MCch2pan)->value.f = 0.7f;
    mixer->getControl(MCch2amp)->value.f = 0.8f;
        mixer->getControl(MCch2st1gain)->value.f = 0.5f;
        mixer->connectEffect(ch2, clip1);
        clip1->controls().amp.value.f = 3.5f;
        clip1->controls().lvl.value.f = 0.8f;
        clip1->controls().cut.value.b = 50;
        clip1->controls().res.value.f = 0.8f;
        clip1->controls().mode.value.b = FmLowPass || FmBandPass;
        clip1->updateFilter();


    if (SoundPlayer::start((int)samplingRate, 2, Mixer8x4::fillSoundBuffer, mixer) == 0) {
        for (var i = 0; i < 2; i++) {
            synth->setNote(note1, 224);
            synth->setNote(note2, 124);
            synth->setNote(note3, 184);
            bass->setNote(note4, 184);
            Sleep(2400);
            synth->setNote(note1, 0);
            synth->setNote(note2, 0);
            synth->setNote(note3, 0);
            bass->setNote(note4, 0);
            Sleep(1200);
        }
        Sleep(1000);
        SoundPlayer::stop();
    }

    assert("Should create sound", synth->isActive());

    DEL_(synth);
    DEL_(bass);
    DEL_(clip1);
    DEL_(mixer);
    DEL_(soundBank);
    DEL_(bassSoundBank);
}

void SynthTest::testCreatePlayerWithDevices() {
    var device = PlayerDevice::create(NULL);
    var player = device->player();
    var mainDevice = player->masterDevice();
    var adapter = player->addAdapter(NEW_(SynthAdapter));
    SynthDevice* synthDevices[5];
    int channelCount = arraysize(synthDevices);
    synthDevices[0] = (SynthDevice*)player->addDevice(adapter, SynthDevices::DeviceSynth);
    synthDevices[1] = (SynthDevice*)player->addDevice(adapter, SynthDevices::DeviceSynth);
    synthDevices[2] = (SynthDevice*)player->addDevice(adapter, SynthDevices::DeviceSynth);
    synthDevices[3] = (SynthDevice*)player->addDevice(adapter, SynthDevices::DeviceSynth);
    synthDevices[4] = (SynthDevice*)player->addDevice(adapter, SynthDevices::DeviceSynth);
    synthDevices[4]->synth()->voiceCount(6);
    var mixerDevice = (MixerDevice*)player->addDevice(adapter, SynthDevices::DeviceMixer);
    assert("Should have 7 devices", player->devices().length() == 7);
    var mixer = (Mixer8x4*)mixerDevice->object();
    mixer->channelCount(channelCount);
    var soundBank = createSoundBank();
    for (var i = 0; i < channelCount; i++) {
        var ch = mixer->getChannel(i);
        mixer->connectInput(ch, synthDevices[i]->synth());
        ch->controls->gain.value.f = 1.0f;
        ch->controls->amp.value.f = 0.5f;
        ch->controls->pan.value.f = 0.5f + 0.3f * (1 - 2 * (i % 2));
        synthDevices[i]->soundBank(soundBank->data());
        synthDevices[i]->setProgram(i);
    }
    //mixer->getChannel(4)->controls->amp.value.f = 0.4f;
    if (SoundPlayer::start((int)samplingRate, 2, Mixer8x4::fillSoundBuffer, mixer) == 0) {
        for (var i = 0; i < 16; i++) {
            // #1    K   Ch HH
            synthDevices[1]->synth()->setNote(pA1,  100);
            synthDevices[3]->synth()->setNote(pE5,  120);
            synthDevices[4]->synth()->setNote(pC4,   80);
            synthDevices[4]->synth()->setNote(pDs4,  70);
            synthDevices[4]->synth()->setNote(pG3,   90);
            Sleep(100);
            synthDevices[1]->synth()->setNote(pA1,    0);
            synthDevices[3]->synth()->setNote(pE5,    0);
            synthDevices[4]->synth()->setNote(pC4,    0);
            synthDevices[4]->synth()->setNote(pDs4,   0);
            synthDevices[4]->synth()->setNote(pG3,    0);
            Sleep(100);
            // #2  B     Ch
            synthDevices[0]->synth()->setNote(pC2,  100);
            synthDevices[3]->synth()->setNote(pE5,  120);
            Sleep(100);
            synthDevices[0]->synth()->setNote(pC2,    0);
            synthDevices[3]->synth()->setNote(pE5,    0);
            Sleep(100);
            // #3      S Ch
            synthDevices[2]->synth()->setNote(pA2,  150);
            synthDevices[3]->synth()->setNote(pE5,  120);
            Sleep(60);
            synthDevices[2]->synth()->setNote(pA2,    0);
            Sleep(40);
            synthDevices[3]->synth()->setNote(pE5,    0);
            Sleep(100);
            // #4  B     Ch
            synthDevices[0]->synth()->setNote(pC2,  100);
            synthDevices[3]->synth()->setNote(pE5,  120);
            Sleep(100);
            synthDevices[0]->synth()->setNote(pC2,    0);
            synthDevices[3]->synth()->setNote(pE5,    0);
            Sleep(100);
        }
        SoundPlayer::stop();
    }

    DEL_(device);
    DEL_(soundBank);
    Player::cleanUp();
}

Stream* SynthTest::createBinaryData() {
    var data = NEW_(Stream);
    PlayerDevice playerDevice(NULL, &SynthTest::playerAdapter_);
    SynthDevice synthDevice(&SynthTest::synthAdapter_);
    BassDevice bassDevice(&SynthTest::synthAdapter_);

    var initData = NEW_(Stream);
    initData->writeFloat(32.0f);
    // 5 channels
    initData->writeByte(5);

    var steps = 128;
    //#region MASTER sequence - 12
    // k kbh kbhc kbhcm kbhcm kbhm kbhm
    var sequence1 = NEW_(Sequence, &playerDevice);
    {
        sequence1->writeHeader();

        sequence1->writeDelta(0);
        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(1)->writeByte(16);
        //sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(5)->writeByte(5)->writeByte(5)->writeByte(8);
        sequence1->writeEOF();

        sequence1->writeDelta(steps);
        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(2)->writeByte(2)->writeByte(2)->writeByte(14);
        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(3)->writeByte(3)->writeByte(3)->writeByte(12);
        sequence1->writeEOF();

        sequence1->writeDelta(steps);
        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(4)->writeByte(4)->writeByte(4)->writeByte(4);
        sequence1->writeEOF();

        sequence1->writeDelta(steps);
        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(5)->writeByte(5)->writeByte(5)->writeByte(8);
        sequence1->writeEOF();

        sequence1->writeDelta(5*steps + 16);
        sequence1->writeEOS();
    }
    //#region Sequence #2 - 64st "drums"
    var sequence2 = NEW_(Sequence, &synthDevice);
    {
        sequence2->writeHeader();
        sequence2->writeDelta(0)->writeCommand(CmdSetProgram)->writeByte(1)->writeCommand(CmdSetNote)->writeByte(pF1)->writeByte(208)->writeEOF();
        sequence2->writeDelta(4)->writeCommand(CmdSetNote)->writeByte(pF1)->writeByte(0)->writeEOF();
        sequence2->writeDelta(12)->writeCommand(CmdSetProgram)->writeByte(2)->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(255)->writeEOF();
        sequence2->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(0)->writeEOF();
        sequence2->writeDelta(14)->writeCommand(CmdSetProgram)->writeByte(1)->writeCommand(CmdSetNote)->writeByte(pF1)->writeByte(192)->writeEOF();
        sequence2->writeDelta(4)->writeCommand(CmdSetNote)->writeByte(pF1)->writeByte(0)->writeEOF();
        sequence2->writeDelta(12)->writeCommand(CmdSetProgram)->writeByte(2)->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(255)->writeEOF();
        sequence2->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(0)->writeEOF();
        sequence2->writeDelta(14)->writeEOS();
    }
    //#region Sequence #3 - 64st "bass"
    var sequence3 = NEW_(Sequence, &bassDevice);
    {
        sequence3->writeHeader();
        sequence3->writeDelta(0);
        sequence3->writeCommand(CmdSetProgram)->writeByte(0);
        sequence3->writeCommand(CmdSetNote)->writeByte(pD1)->writeByte(128);
        sequence3->writeEOF();
        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pD1)->writeByte(0);
        sequence3->writeEOF();

        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(128);
        sequence3->writeEOF();
        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(0);
        sequence3->writeEOF();
    
        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pF1)->writeByte(128);
        sequence3->writeEOF();
        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pF1)->writeByte(0);
        sequence3->writeEOF();

        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(128);
        sequence3->writeEOF();
        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(0);
        sequence3->writeEOF();

        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pC1)->writeByte(128);
        sequence3->writeEOF();
        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pC1)->writeByte(0);
        sequence3->writeEOF();

        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(128);
        sequence3->writeEOF();
        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(0);
        sequence3->writeEOF();

        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pG1)->writeByte(128);
        sequence3->writeEOF();
        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pG1)->writeByte(0);
        sequence3->writeEOF();

        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(128);
        sequence3->writeEOF();
        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(0);
        sequence3->writeEOF();

        sequence3->writeDelta(4);
        sequence3->writeEOS();
    }
    
    //#region Sequence #4 - 64st "hihat"
    var sequence4 = NEW_(Sequence, &synthDevice);
    {
        sequence4->writeHeader();
        // 0               1               2               3
        // 0...1...2...3...0...1...2...3...0...1...2...3...0...1...2...3...
        // c.......oooo....c.......ooo...c.....c...ooo.c...c.......ooo.c...
        // 01      74      41      73    31    51  33  11  31      73  11  3
        // #0.0
        sequence4->writeDelta(0)->writeCommand(CmdSetNote)->writeByte(pNone)->writeByte(180);sequence4->writeEOF();
        sequence4->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pNone)->writeByte(  0); sequence4->writeEOF();

        sequence4->writeDelta(7)->writeCommand(CmdSetNote)->writeByte(pNone)->writeByte(220); sequence4->writeEOF();
        sequence4->writeDelta(4)->writeCommand(CmdSetNote)->writeByte(pNone)->writeByte(  0); sequence4->writeEOF();

        sequence4->writeDelta(4)->writeCommand(CmdSetNote)->writeByte(pNone)->writeByte(180); sequence4->writeEOF();
        sequence4->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pNone)->writeByte(  0); sequence4->writeEOF();

        sequence4->writeDelta(7)->writeCommand(CmdSetNote)->writeByte(pNone)->writeByte(220); sequence4->writeEOF();
        sequence4->writeDelta(3)->writeCommand(CmdSetNote)->writeByte(pNone)->writeByte(  0); sequence4->writeEOF();

        sequence4->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pNone)->writeByte(160); sequence4->writeEOF();
        sequence4->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pNone)->writeByte(  0); sequence4->writeEOF();

        sequence4->writeDelta(6)->writeCommand(CmdSetNote)->writeByte(pNone)->writeByte(180); sequence4->writeEOF();
        sequence4->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pNone)->writeByte(  0); sequence4->writeEOF();

        sequence4->writeDelta(3)->writeCommand(CmdSetNote)->writeByte(pNone)->writeByte(220); sequence4->writeEOF();
        sequence4->writeDelta(3)->writeCommand(CmdSetNote)->writeByte(pNone)->writeByte(  0); sequence4->writeEOF();

        sequence4->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pNone)->writeByte(160); sequence4->writeEOF();
        sequence4->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pNone)->writeByte(  0); sequence4->writeEOF();

        sequence4->writeDelta(3)->writeCommand(CmdSetNote)->writeByte(pNone)->writeByte(180); sequence4->writeEOF();
        sequence4->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pNone)->writeByte(0  ); sequence4->writeEOF();

        sequence4->writeDelta(7)->writeCommand(CmdSetNote)->writeByte(pNone)->writeByte(220); sequence4->writeEOF();
        sequence4->writeDelta(3)->writeCommand(CmdSetNote)->writeByte(pNone)->writeByte(  0); sequence4->writeEOF();

        sequence4->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pNone)->writeByte(170); sequence4->writeEOF();
        sequence4->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pNone)->writeByte(  0); sequence4->writeEOF();

        sequence4->writeDelta(3);
        sequence4->writeEOS();
    }
    //#region Sequence #3 - 128st "chords"
    var sequence5 = NEW_(Sequence, &synthDevice);
    {
        sequence5->writeHeader();
        // Frame #1.1
        sequence5->writeDelta(0);
        sequence5->writeCommand(CmdSetProgram)->writeByte(4);
        sequence5->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(128);
        sequence5->writeCommand(CmdSetNote)->writeByte(pF3)->writeByte(128);
        sequence5->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(128);
        sequence5->writeEOF();
        sequence5->writeDelta(8);
        sequence5->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(0);
        sequence5->writeCommand(CmdSetNote)->writeByte(pF3)->writeByte(0);
        sequence5->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(0);
        sequence5->writeEOF();
        sequence5->writeDelta(24);
        sequence5->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(128);
        sequence5->writeCommand(CmdSetNote)->writeByte(pF3)->writeByte(128);
        sequence5->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(128);
        sequence5->writeEOF();
        sequence5->writeDelta(8);
        sequence5->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(0);
        sequence5->writeCommand(CmdSetNote)->writeByte(pF3)->writeByte(0);
        sequence5->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(0);
        sequence5->writeEOF();
        sequence5->writeDelta(24);
        sequence5->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(128);
        sequence5->writeCommand(CmdSetNote)->writeByte(pE3)->writeByte(128);
        sequence5->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(128);
        sequence5->writeEOF();
        sequence5->writeDelta(8);
        sequence5->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(0);
        sequence5->writeCommand(CmdSetNote)->writeByte(pE3)->writeByte(0);
        sequence5->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(0);
        sequence5->writeEOF();
        sequence5->writeDelta(24);
        sequence5->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(128);
        sequence5->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(128);
        sequence5->writeCommand(CmdSetNote)->writeByte(pH3)->writeByte(128);
        sequence5->writeEOF();
        sequence5->writeDelta(8);
        sequence5->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(0);
        sequence5->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(0);
        sequence5->writeCommand(CmdSetNote)->writeByte(pH3)->writeByte(0);
        sequence5->writeEOF();
        sequence5->writeDelta(24);
        sequence5->writeEOS();
    }
    //#region Sequence #3 - 64st "mono"
    var sequence6 = NEW_(Sequence, &synthDevice);
    {
        sequence6->writeHeader();
        // #1
        sequence6->writeDelta(0)->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pD3)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pD3)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(0)->writeEOF();

        // #2
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pF3)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pF3)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pF3)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pF3)->writeByte(0)->writeEOF();

        // #3
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pC3)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pC3)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pC3)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pC3)->writeByte(0)->writeEOF();

        
        // #3
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pG2)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pG2)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pG2)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pG2)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeEOS();
    }

    var adapterList = NEW_(Stream);
    // 2 adapters
    adapterList->writeByte(2);
    // 1st adapter: PlayerAdapter
    {
        adapterList->writeByte(SynthTest::playerAdapter_.getInfo()->id);
        // no init data
        // no devices
        adapterList->writeByte(0);
    }
    // 2nd adapter: SynthAdapter
    {
        adapterList->writeByte(SynthTest::synthAdapter_.getInfo()->id);
        adapterList->writeWord(48000);
        // 10 devices
        adapterList->writeByte(10);
        adapterList->writeByte(SynthDevices::DeviceSynth);  //dev #1
            adapterList->writeByte(1);  // voice count
            adapterList->writeByte(0);  // data block id for sound bank 
            adapterList->writeByte(1);  // program

        adapterList->writeByte(SynthDevices::DeviceBass);  //dev #2
            adapterList->writeByte(1);  // voice count
            adapterList->writeByte(1);  // data block id for sound bank 
            adapterList->writeByte(0);  // program

        adapterList->writeByte(SynthDevices::DeviceSynth);  //dev #3
            adapterList->writeByte(1);  // voice count
            adapterList->writeByte(0);  // data block id for sound bank 
            adapterList->writeByte(3);  // program

        adapterList->writeByte(SynthDevices::DeviceSynth);  //dev #4
            adapterList->writeByte(6);  // voice count
            adapterList->writeByte(0);  // data block id for sound bank 
            adapterList->writeByte(4);  // program

        adapterList->writeByte(SynthDevices::DeviceSynth);  //dev #5
            adapterList->writeByte(1);  // voice count
            adapterList->writeByte(0);  // data block id for sound bank 
            adapterList->writeByte(5);  // program

        adapterList->writeByte(SynthDevices::DeviceClip);  //dev #6
            adapterList->writeFloat(3.1f);      // amp
            adapterList->writeByte(220);        // lvl
            adapterList->writeByte(30);         // cut
            adapterList->writeByte(180);        // res
            adapterList->writeByte(FmLowPass || FmBandPass /*|| FmHighPass */ );    // mode

        adapterList->writeByte(SynthDevices::DeviceDelay);  //dev #7
            adapterList->writeByte(160);        // feedbackL
            adapterList->writeFloat(120.0f);    // delayL
            adapterList->writeByte(140);        // feedbackR
            adapterList->writeFloat(240.0f);    // delayR
            adapterList->writeByte(120);        // mixL
            adapterList->writeByte(120);        // mixR

        adapterList->writeByte(SynthDevices::DeviceDelay);  //dev #8
            adapterList->writeByte(140);        // feedbackL
            adapterList->writeFloat(64.0f);    // delayL
            adapterList->writeByte(140);        // feedbackR
            adapterList->writeFloat( 96.0f);    // delayR
            adapterList->writeByte(120);        // mixL
            adapterList->writeByte(220);        // mixR

        adapterList->writeByte(SynthDevices::DeviceDelay);  //dev #9
            adapterList->writeByte( 40);        // feedbackl
            adapterList->writeFloat( 11.5f);    // delayL
            adapterList->writeByte(120);        // feedbackR
            adapterList->writeFloat(380.1f);    // delayR
            adapterList->writeByte(200);        // mixL
            adapterList->writeByte(180);        // mixR

        adapterList->writeByte(SynthDevices::DeviceMixer);  //dev #10
            adapterList->writeByte(5);
            
        // channel #1
            adapterList->writeByte(255);    // gain#1 - kick+snare
            adapterList->writeByte( 50);    // pan#1
            adapterList->writeByte(240);    // amp#1
            adapterList->writeByte(  0);    // stage count

        // channel #2
            adapterList->writeByte(45);     // gain#2 - bass
            adapterList->writeByte( 60);    // pan#2
            adapterList->writeByte(120);    // amp#2
            adapterList->writeByte(1);      // stage count
                adapterList->writeByte(100);// stage gain

        // channel #3
            adapterList->writeByte(200);    // gain#3 - hihat
            adapterList->writeByte( 40);    // pan#3
            adapterList->writeByte(200);    // amp#3
            adapterList->writeByte(0);      // stage count

        // channel #4                                            
            adapterList->writeByte(200);    // gain#4 - chords
            adapterList->writeByte( 30);    // pan#4
            adapterList->writeByte( 80);    // amp#4
            adapterList->writeByte(2);      // stage count
                adapterList->writeByte(180);  // stage gain
                adapterList->writeByte( 80);  // stage gain

        // channel #5
            adapterList->writeByte(220);    // gain#3 - mono
            adapterList->writeByte( 40);    // pan#5
            adapterList->writeByte( 70);    // amp#5
            adapterList->writeByte(1);      // stage count
                adapterList->writeByte(220);  // stage gain
                
        adapterList->writeByte(1);  // channel1 input
        adapterList->writeByte(2);  // channel2 input
            adapterList->writeByte(6);  // channel3.stage1 input
        adapterList->writeByte(3);  // channel3 input
        adapterList->writeByte(4);  // channel4 input
            adapterList->writeByte(7);  // channel4.stage1 input
            adapterList->writeByte(8);  // channel4.stage2 input
        adapterList->writeByte(5);  // channel5 input
            adapterList->writeByte(9);  // channel1.stage1 input

    }

    var sequenceList = NEW_(Stream);
    sequenceList->writeByte(6);
    sequenceList->writeWord((word)sequence1->length());
    sequenceList->writeWord((word)sequence2->length());
    sequenceList->writeWord((word)sequence3->length());
    sequenceList->writeWord((word)sequence4->length());
    sequenceList->writeWord((word)sequence5->length());
    sequenceList->writeWord((word)sequence6->length());
    sequenceList->writeStream(sequence1);
    sequenceList->writeStream(sequence2);
    sequenceList->writeStream(sequence3);
    sequenceList->writeStream(sequence4);
    sequenceList->writeStream(sequence5);
    sequenceList->writeStream(sequence6);
    DEL_(sequence1);
    DEL_(sequence2);
    DEL_(sequence3);
    DEL_(sequence4);
    DEL_(sequence5);
    DEL_(sequence6);

    var soundBank = createSoundBank();
    var bassSoundBank = createBassSoundBank();

    // header
    // 3 data blocks
    data->writeWord(5);
    data->writeDword(initData->length());
    data->writeDword(adapterList->length());
    data->writeDword(sequenceList->length());
    data->writeDword(soundBank->length());
    data->writeDword(bassSoundBank->length());
    

    data->writeStream(initData);
    DEL_(initData);
    data->writeStream(adapterList);
    DEL_(adapterList);
    data->writeStream(sequenceList);
    DEL_(sequenceList);
    data->writeStream(soundBank);
    DEL_(soundBank);
    data->writeStream(bassSoundBank);
    DEL_(bassSoundBank);

    return data;
}

void SynthTest::testLoadBinary() {
    var bin = createBinaryData();
    Player::addAdapter(NEW_(SynthAdapter));
    var data = bin->extract();
    DEL_(bin);
    var device = PlayerDevice::create(&data);
    var player = device->player();
    player->assignChannel(0, (Sequence*)player->sequences().get(0), 0, 0);
    assert("Should have 32.0 fps", player->refreshRate() == 32.0f);
    assert("Should have 11 devices", player->devices().length() == 11);
    assert("Should have PlayerDevice as #1", ((Device*)player->devices().get(0))->type() == PlayerDevices::DevicePlayer);
    assert("Should have SynthDevice as #2, #4, #5 and #6",
        ((Device*)player->devices().get(1))->type() ==SynthDevices::DeviceSynth &&
        ((Device*)player->devices().get(3))->type() == SynthDevices::DeviceSynth &&
        ((Device*)player->devices().get(4))->type() == SynthDevices::DeviceSynth &&
        ((Device*)player->devices().get(5))->type() == SynthDevices::DeviceSynth);
    assert("Should have BassDevice as #3",
        ((Device*)player->devices().get(2))->type() == SynthDevices::DeviceBass);
    assert("Should have ClipDevice as #7", ((Device*)player->devices().get(6))->type() == SynthDevices::DeviceClip);
    assert("Should have DelayDevice as #8, #9 and #10",
        ((Device*)player->devices().get(7))->type() == SynthDevices::DeviceDelay &&
        ((Device*)player->devices().get(8))->type() == SynthDevices::DeviceDelay &&
        ((Device*)player->devices().get(9))->type() == SynthDevices::DeviceDelay);
    assert("Should have MixerDevice as #11", ((Device*)player->devices().get(10))->type() == SynthDevices::DeviceMixer);
    assert("Should have 6 sequences", player->sequences().length() == 6);
    assert("Should have 2 data-blocks", player->dataBlocks().length() == 2);
    int ix = 0;
    var mixer = (Mixer8x4*)((Device*)player->devices().search(NULL, ix,
        [](void* value, UINT32 ix, Collection* collection, void* key) {
            return ((Device*)value)->type() == SynthDevices::DeviceMixer ? 0 : 1;
        }
    ))->object();
    if (SoundPlayer::start((int)samplingRate, 2, Mixer8x4::fillSoundBuffer, mixer) == 0) {
        player->useThread();
        player->start();
        while (device->isActive()) {
            Sleep(3);
        }
        assert("Should run to the end", true);
        SoundPlayer::stop();
    }

    DEL_(device);
    Player::cleanUp();
}

void SynthTest::testCreateBinary() {
    var bin = createBinaryData();
    Player::addAdapter(NEW_(SynthAdapter));
    var data = bin->data();
    var device = PlayerDevice::create(&data);
    var player = device->player();
    var stream = player->save();

    var cons = getConsole();
    cons->printf("Expected\n");
    cons->dump(bin->data(), bin->length(), 16);
    cons->printf("---\nReceived\n");
    cons->dump(stream->data(), stream->length(), 16);
    // compare bin and stream;
    assert("Should be same long", bin->length() == stream->length());
    var isMatch = true;
    stream->reset();
    bin->reset();
    cons->printf("Binary comparison\n");
    for (var i = 0; i < bin->length(); i++) {
        var ca = bin->readByte();
        var cb = stream->readByte();
        if (ca != cb) {
            cons->printf(" - %04X: %02X %02X\n", i, ca, cb);
            isMatch = false;
            break;
        }
    }
    assert("Should be binary identical", isMatch);

    bin->extract();
    DEL_(stream);
    DEL_(bin);
    DEL_(device);
    Player::cleanUp();
}

void SynthTest::testSaveToWave() {
    var bin = createBinaryData();
    var synthAdapter = (SynthAdapter*)Player::addAdapter(NEW_(SynthAdapter));
    var data = bin->extract();
    DEL_(bin);
    var device = PlayerDevice::create(&data);
    var player = device->player();
    var seq = (Sequence*)player->sequences().get(0);
    player->assignChannel(0, seq, 0, 0);
    var frames = seq->toFrames(player->masterDevice());
    var totalFrameCount = 0;
    for (var i = 0; i < frames->length(); i++) {
        var frame = (Frame*)frames->get(i);
        totalFrameCount += frame->delta_;
    }
    int ix = 0;
    var mixer = (Mixer8x4*)((Device*)player->devices().search(NULL, ix,
        [](void* value, UINT32 ix, Collection* collection, void* key) {
            return ((Device*)value)->type() == SynthDevices::DeviceMixer ? 0 : 1;
        }
    ))->object();
    var samplesPerFrame = synthAdapter->samplingRate / player->refreshRate();
    int bufferSizeInSamples = 16384;
    int bufferSize = 2 * sizeof(short) * bufferSizeInSamples;
    var buffer = MALLOC(short, bufferSize);
    var offset = buffer;
    var wave = NEW_(WaveFmt, "ouput.wav", (int)samplingRate, 2, 16);
    var cons = getConsole();
    var frameCounter = 0.0f;
    var frameCount = 0;
    int totalCount = 0;
    do {
        int remainingSamples = bufferSizeInSamples;
        memset(buffer, 0, bufferSize);
        var offset = buffer;
        while (remainingSamples > 0) {
            var count = remainingSamples > SAMPLE_BUFFER_SIZE ? SAMPLE_BUFFER_SIZE : remainingSamples;
            if (frameCounter <= 0) {
                if (!player->run(1)) break;
                frameCount++;
                frameCounter += samplesPerFrame;
            }
            if (frameCounter < count) {
                count = (int)frameCounter;
            }
            Mixer8x4::fillSoundBuffer(offset, count, mixer);
            offset += 2 * count;
            totalCount += count;
            cons->showCursor(false);
            cons->printf("% 5d / % 5d", frameCount, totalFrameCount);
            cons->movexy(-13, 0);
            remainingSamples -= count;
            frameCounter -= count;
        }
        wave->write((byte*)buffer, (int)(2*(offset - buffer)));
    } while (player->masterChannel()->isActive());
    cons->printf("\n%d samples saved\n", totalCount);

    assert("Should render correct number of frames", frameCount == totalFrameCount);
    assert("Should create correct number of samples", totalCount == frameCount*samplesPerFrame);

    wave->close();
    DEL_(wave);
    FREE(buffer);
    DEL_(device);
    Player::cleanUp();
}

void SynthTest::testLoadXm() {

}

int _main(NS_FW_BASE::Map* args) {
#ifdef _DEBUG
    Debug::initialize(/*DEBUG_UNICODE | DEBUG_MEMORY*/);
#endif

    LOG("Player tests\n");
    var tests = NEW_(SynthTest);

    //tests->test("Generate Sound", (TestMethod) & (SynthTest::testGenerateSoundSimple));
    //tests->test("Generate Sound via Mixer", (TestMethod) & (SynthTest::testGenerateSoundMixer));
    //tests->test("Create player with devices", (TestMethod) & (SynthTest::testCreatePlayerWithDevices));
    tests->test("Load from binary", (TestMethod) & (SynthTest::testLoadBinary));
    //tests->test("Create binary", (TestMethod) & (SynthTest::testCreateBinary));
    //tests->test("Save to wave", (TestMethod) & (SynthTest::testSaveToWave));
    //tests->test("Load XM module", (TestMethod) & (SynthTest::testLoadXm));
    //tests->test("Filter comparison", (TestMethod) & (SynthTest::testFilterSound));

    tests->displayFinalResults();

    DEL_(tests);

    return 0;
}

//Player* loadXm(char* path) {
//    Player::registerAdapter(SynthAdapter::Info);
//    var player = (Player*)Player::creator(NULL);
//    var adapter = player->addAdapter(SynthAdapter::Info.id, 1);
//    adapter->player(player);
//    var soundBank = createSoundBank();
//    var xmLoader = NEW_(XmLoader, player, soundBank);
//    var error = xmLoader->load(path);
//    if (error != 0) {
//        printf("Error loading xm (%d)", error);
//    }
////    else {
////#ifdef _DEBUG
////        for (var i = 0; i < xmLoader->patterns().length(); i++) {
////            var buffer = xmLoader->printPattern(i);
////            printf("Pattern #%d\n%s", (i+1), buffer);
////            FREE(buffer);
////        }
////#endif
////    }
//    player->sequences().forEach([](void* item, UINT32 ix, Collection* col, void* args) {
//        var seq = (Sequence*)item;
//        var text = seq->print();
//        printf("Seq #%d\n%s", ix+1, text);
//        FREE(text);
//        return 1;
//    });
//    DEL_(xmLoader);
//
//    ((SynthDevice*)adapter->getDevice(0))->synth()->setVoiceCount(2);
//    ((SynthDevice*)adapter->getDevice(3))->synth()->setVoiceCount(3);
//    player->masterChannel()->assign(0, (Sequence*)player->sequences().get(0));
//
//    return player;
//}
//
//int testLoadXm() {
//    const char* fileName = "min";
//    var fileNameWithExtension = str_format("%s.xm", fileName);
//    var player = loadXm(fileNameWithExtension);
//    FREE(fileNameWithExtension);
//    const float bpm = 1.5f * 127.0f / 3.75f;
//    player->setRefreshRate(bpm);
//#ifdef SAVE_TO_WAVE
//    saveToWave(str_format("%s.wav", fileName), player);
//#else
//    player->masterChannel()->loopCount(2);
//    player->masterChannel()->reset();
//    playback(player, 2);
//#endif
//    DEL_(player);
//    return 0;
//}