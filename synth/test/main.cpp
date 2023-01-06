#include <math.h>
#include "test.h"
#include "consoleapp.h"
#include "soundlib/src/soundplayer.h"
#include "soundlib/src/wavfmt.h"
#include "player-lib.h"
#include "player/src/player-adapter.h"
//#include "synth/src/module/synth-controls.h"
#include "synth/src/device/synth-device.h"
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
    Stream* createBinaryData();
    void createTestSequence(PlayerDevice* device);
    //Stream* createSynthInitData();
public:
    static float buffer_[4096];
    float samplingRate;

    SynthTest();
    void testGenerateSoundSimple();
    void testGenerateSoundMixer();
    void testCreatePlayerWithDevices();
    void testLoadBinary();
    void testCreateBinary();
    void testSaveToWave();
    //void testCreatePlayer();
    //void testCreateSequence();
    //void testCreateSequenceFromFrames();
    //void testLoadFromBinary();
    //void testCreateBinary();
    //void testRunPlayer();
    //int testSaveToWave();
    //int testLoadXm();
};

float SynthTest::buffer_[4096];

void simpleSoundCallback(short* buffer, int sampleCount, void* context) {
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
//    var sequence1 = (Sequence*)playerDevice_->sequences().getAt(0);
//    var sequence2 = (Sequence*)playerDevice_->sequences().getAt(1);
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

Stream* SynthTest::createSoundBank() {
    var bass = NEW_(Stream);
    {
        bass->writeByte(43);
        bass->writeByte(SCamp)->writeByte(185);         // amp
        bass->writeByte(SCenv1amp)->writeFloat(1.0f);   // amp env
        bass->writeByte(SCenv1dc)->writeFloat(0.0f);
        bass->writeByte(SCenv1atk)->writeByte(12);
        bass->writeByte(SCenv1dec)->writeByte(48);
        bass->writeByte(SCenv1sus)->writeByte(80);
        bass->writeByte(SCenv1rel)->writeByte(42);
        bass->writeByte(SCenv2amp)->writeFloat(0.2f);   // psw env
        bass->writeByte(SCenv2dc)->writeFloat(0.6f);
        bass->writeByte(SCenv2atk)->writeByte(0);
        bass->writeByte(SCenv2dec)->writeByte(54);
        bass->writeByte(SCenv2sus)->writeByte(108);
        bass->writeByte(SCenv2rel)->writeByte(40);
        bass->writeByte(SCenv3amp)->writeFloat(1.0f);   // cut env
        bass->writeByte(SCenv3dc)->writeFloat(0.0f);
        bass->writeByte(SCenv3atk)->writeByte(12);
        bass->writeByte(SCenv3dec)->writeByte(82);
        bass->writeByte(SCenv3sus)->writeByte(20);
        bass->writeByte(SCenv3rel)->writeByte(42);
        bass->writeByte(SCenv4amp)->writeFloat(0.0f);   // fm env
        bass->writeByte(SCenv4dc)->writeFloat(0.0f);
        bass->writeByte(SCenv4atk)->writeByte(0);
        bass->writeByte(SCenv4dec)->writeByte(0);
        bass->writeByte(SCenv4sus)->writeByte(0);
        bass->writeByte(SCenv4rel)->writeByte(0);
        bass->writeByte(SClfo1amp)->writeFloat(0.1f);   // lfo1
        bass->writeByte(SClfo1fre)->writeFloat(2.0f);
        bass->writeByte(SClfo2amp)->writeFloat(1.0f);   // lfo2
        bass->writeByte(SClfo2fre)->writeFloat(3.8f);
        bass->writeByte(SCosc1amp)->writeFloat(0.6f);   // osc1
        bass->writeByte(SCosc1fre)->writeFloat(0.0f);
        bass->writeByte(SCosc1tune)->writeByte(0);
        bass->writeByte(SCosc1psw)->writeByte(0);
        bass->writeByte(SCosc1wave)->writeByte(WfPulse);
        bass->writeByte(SCosc2amp)->writeFloat(0.3f);   // osc2
        bass->writeByte(SCosc2fre)->writeFloat(0.1f);
        bass->writeByte(SCosc2tune)->writeByte(12);
        bass->writeByte(SCosc2psw)->writeByte(10);
        bass->writeByte(SCosc2wave)->writeByte(WfSaw);
        bass->writeByte(SCflt1cut)->writeByte(30);   // flt
        bass->writeByte(SCflt1res)->writeByte(80);
        bass->writeByte(SCflt1mod)->writeByte(40);
        bass->writeByte(SCflt1mode)->writeByte(FmLowPass || FmBandPass);
    }
    var kick = NEW_(Stream);
    {
        kick->writeByte(39);
        kick->writeByte(SCamp)->writeByte(255);
        kick->writeByte(SCenv1amp)->writeFloat(1.0f);   // amo env
        kick->writeByte(SCenv1atk)->writeByte(2);
        kick->writeByte(SCenv1dec)->writeByte(82);
        kick->writeByte(SCenv1sus)->writeByte(140);
        kick->writeByte(SCenv1rel)->writeByte(26);
        kick->writeByte(SCenv2amp)->writeFloat(0.0f);   // psw env
        kick->writeByte(SCenv2atk)->writeByte(0);
        kick->writeByte(SCenv2dec)->writeByte(0);
        kick->writeByte(SCenv2sus)->writeByte(0);
        kick->writeByte(SCenv2rel)->writeByte(0);
        kick->writeByte(SCenv3amp)->writeFloat(1.0f);   // cutoff env
        kick->writeByte(SCenv3atk)->writeByte(1);
        kick->writeByte(SCenv3dec)->writeByte(8);
        kick->writeByte(SCenv3sus)->writeByte(1);
        kick->writeByte(SCenv3rel)->writeByte(2);
        kick->writeByte(SCenv4amp)->writeFloat(120.0f); // fm envelope
        kick->writeByte(SCenv4atk)->writeByte( 1);
        kick->writeByte(SCenv4dec)->writeByte(42);
        kick->writeByte(SCenv4sus)->writeByte( 4);
        kick->writeByte(SCenv4rel)->writeByte( 40);
        kick->writeByte(SClfo1amp)->writeFloat(0.3f);   // lfo1
        kick->writeByte(SClfo1fre)->writeFloat(22.2f);
        kick->writeByte(SClfo2amp)->writeFloat(1.1f);   // lfo2
        kick->writeByte(SClfo2fre)->writeFloat(0.3f);
        kick->writeByte(SCosc1amp)->writeFloat(0.9f);
        kick->writeByte(SCosc1fre)->writeFloat(0.0f);
        kick->writeByte(SCosc1tune)->writeByte(0);
        kick->writeByte(SCosc1psw)->writeByte(0);
        kick->writeByte(SCosc1wave)->writeByte(WfSinus);
        kick->writeByte(SCosc2amp)->writeFloat(0.4f);
        kick->writeByte(SCosc2fre)->writeFloat(9210.0f);
        kick->writeByte(SCosc2tune)->writeByte(0);
        kick->writeByte(SCosc2psw)->writeByte(12);
        kick->writeByte(SCosc2wave)->writeByte(WfNoise);
        kick->writeByte(SCflt1cut)->writeByte(0);
        kick->writeByte(SCflt1res)->writeByte(60);
        kick->writeByte(SCflt1mod)->writeByte(250);
        kick->writeByte(SCflt1mode)->writeByte(FmLowPass);
    }
    var snare = NEW_(Stream);
    {
        snare->writeByte(39);
        snare->writeByte(SCamp)->writeByte(255);        // amp
        snare->writeByte(SCenv1amp)->writeFloat(1.0f);  // amp env
        snare->writeByte(SCenv1atk)->writeByte(1);
        snare->writeByte(SCenv1dec)->writeByte(30);
        snare->writeByte(SCenv1sus)->writeByte(80);
        snare->writeByte(SCenv1rel)->writeByte(60);
        snare->writeByte(SCenv2amp)->writeFloat(0.0f);  // psw env
        snare->writeByte(SCenv2atk)->writeByte(0);
        snare->writeByte(SCenv2dec)->writeByte(0);
        snare->writeByte(SCenv2sus)->writeByte(0);
        snare->writeByte(SCenv2rel)->writeByte(0);
        snare->writeByte(SCenv3amp)->writeFloat(1.0f);  // cut env
        snare->writeByte(SCenv3atk)->writeByte(2);
        snare->writeByte(SCenv3dec)->writeByte(40);
        snare->writeByte(SCenv3sus)->writeByte(100);
        snare->writeByte(SCenv3rel)->writeByte(20);
        snare->writeByte(SCenv4amp)->writeFloat(120.0f);// fm env
        snare->writeByte(SCenv4atk)->writeByte(1);
        snare->writeByte(SCenv4dec)->writeByte(10);
        snare->writeByte(SCenv4sus)->writeByte(2);
        snare->writeByte(SCenv4rel)->writeByte(120);
        snare->writeByte(SClfo1amp)->writeFloat(0.2f);  // lfo1
        snare->writeByte(SClfo1fre)->writeFloat(33.2f);
        snare->writeByte(SClfo2amp)->writeFloat(90.0f); // lfo2
        snare->writeByte(SClfo2fre)->writeFloat(153.3f);
        snare->writeByte(SCosc1amp)->writeFloat(0.9f);  // osc1
        snare->writeByte(SCosc1fre)->writeFloat(121.0f);
        snare->writeByte(SCosc1tune)->writeByte(0);
        snare->writeByte(SCosc1psw)->writeByte(80);
        snare->writeByte(SCosc1wave)->writeByte(WfSinus);
        snare->writeByte(SCosc2amp)->writeFloat(0.2f);  // osc2
        snare->writeByte(SCosc2fre)->writeFloat(9163.63f);
        snare->writeByte(SCosc2tune)->writeByte(0);
        snare->writeByte(SCosc2psw)->writeByte(0);
        snare->writeByte(SCosc2wave)->writeByte(WfNoise);
        snare->writeByte(SCflt1cut)->writeByte(250);    // flt
        snare->writeByte(SCflt1res)->writeByte( 50);
        snare->writeByte(SCflt1mod)->writeByte(240);
        snare->writeByte(SCflt1mode)->writeByte(FmLowPass || FmBandPass);
    }
    var hihat = NEW_(Stream);
    {
        hihat->writeByte(39);
        hihat->writeByte(SCamp)->writeByte(215);        // amp
        hihat->writeByte(SCenv1amp)->writeFloat(1.0f);  // amp env
        hihat->writeByte(SCenv1atk)->writeByte(0);
        hihat->writeByte(SCenv1dec)->writeByte(24);
        hihat->writeByte(SCenv1sus)->writeByte(40);
        hihat->writeByte(SCenv1rel)->writeByte(20);
        hihat->writeByte(SCenv2amp)->writeFloat(0.4f);  // psw env
        hihat->writeByte(SCenv2atk)->writeByte(0);
        hihat->writeByte(SCenv2dec)->writeByte(20);
        hihat->writeByte(SCenv2sus)->writeByte(40);
        hihat->writeByte(SCenv2rel)->writeByte(120);
        hihat->writeByte(SCenv3amp)->writeFloat(1.0f);  // cut env
        hihat->writeByte(SCenv3atk)->writeByte(8);
        hihat->writeByte(SCenv3dec)->writeByte(32);
        hihat->writeByte(SCenv3sus)->writeByte(200);
        hihat->writeByte(SCenv3rel)->writeByte(220);
        hihat->writeByte(SCenv4amp)->writeFloat(2500.0f);// fm env
        hihat->writeByte(SCenv4atk)->writeByte(4);
        hihat->writeByte(SCenv4dec)->writeByte(2);
        hihat->writeByte(SCenv4sus)->writeByte(1);
        hihat->writeByte(SCenv4rel)->writeByte(220);
        hihat->writeByte(SClfo1amp)->writeFloat(1.0f);  // lfo1
        hihat->writeByte(SClfo1fre)->writeFloat(1410.1f);
        hihat->writeByte(SClfo2amp)->writeFloat(500.1f); // lfo2
        hihat->writeByte(SClfo2fre)->writeFloat(805.1f);
        hihat->writeByte(SCosc1amp)->writeFloat(0.4f);  // osc1
        hihat->writeByte(SCosc1fre)->writeFloat(1300.0f);
        hihat->writeByte(SCosc1tune)->writeByte(0);
        hihat->writeByte(SCosc1psw)->writeByte(80);
        hihat->writeByte(SCosc1wave)->writeByte(WfPulse);
        hihat->writeByte(SCosc2amp)->writeFloat(0.7f);  // osc2
        hihat->writeByte(SCosc2fre)->writeFloat(9163.63f);
        hihat->writeByte(SCosc2tune)->writeByte(0);
        hihat->writeByte(SCosc2psw)->writeByte(80);
        hihat->writeByte(SCosc2wave)->writeByte(WfNoise);
        hihat->writeByte(SCflt1cut)->writeByte(180);    // flt
        hihat->writeByte(SCflt1res)->writeByte(10);
        hihat->writeByte(SCflt1mod)->writeByte(250);
        hihat->writeByte(SCflt1mode)->writeByte(FmHighPass);
    }
    var chords = NEW_(Stream);
    {
        chords->writeByte(43);
        chords->writeByte(SCamp)->writeByte(255);         // amp
        chords->writeByte(SCenv1amp)->writeFloat(1.0f);   // amp env
        chords->writeByte(SCenv1dc)->writeFloat(0.0f);
        chords->writeByte(SCenv1atk)->writeByte(12);
        chords->writeByte(SCenv1dec)->writeByte(48);
        chords->writeByte(SCenv1sus)->writeByte(100);
        chords->writeByte(SCenv1rel)->writeByte(42);
        chords->writeByte(SCenv2amp)->writeFloat(0.1f);   // psw env
        chords->writeByte(SCenv2dc)->writeFloat(0.45f);
        chords->writeByte(SCenv2atk)->writeByte(2);
        chords->writeByte(SCenv2dec)->writeByte(84);
        chords->writeByte(SCenv2sus)->writeByte(128);
        chords->writeByte(SCenv2rel)->writeByte(40);
        chords->writeByte(SCenv3amp)->writeFloat(1.0f);   // cut env
        chords->writeByte(SCenv3dc)->writeFloat(0.0f);
        chords->writeByte(SCenv3atk)->writeByte(22);
        chords->writeByte(SCenv3dec)->writeByte(82);
        chords->writeByte(SCenv3sus)->writeByte(60);
        chords->writeByte(SCenv3rel)->writeByte(162);
        chords->writeByte(SCenv4amp)->writeFloat(0.0f);   // fm env
        chords->writeByte(SCenv4dc)->writeFloat(0.0f);
        chords->writeByte(SCenv4atk)->writeByte(0);
        chords->writeByte(SCenv4dec)->writeByte(0);
        chords->writeByte(SCenv4sus)->writeByte(0);
        chords->writeByte(SCenv4rel)->writeByte(0);
        chords->writeByte(SClfo1amp)->writeFloat(0.3f);   // lfo1
        chords->writeByte(SClfo1fre)->writeFloat(4.1f);
        chords->writeByte(SClfo2amp)->writeFloat(6.4f);   // lfo2
        chords->writeByte(SClfo2fre)->writeFloat(4.8f);
        chords->writeByte(SCosc1amp)->writeFloat(0.3f);   // osc1
        chords->writeByte(SCosc1fre)->writeFloat(0.0f);
        chords->writeByte(SCosc1tune)->writeByte(0);
        chords->writeByte(SCosc1psw)->writeByte(120);
        chords->writeByte(SCosc1wave)->writeByte(WfSaw);
        chords->writeByte(SCosc2amp)->writeFloat(0.6f);   // osc2
        chords->writeByte(SCosc2fre)->writeFloat(1.0f);
        chords->writeByte(SCosc2tune)->writeByte(12);
        chords->writeByte(SCosc2psw)->writeByte(140);
        chords->writeByte(SCosc2wave)->writeByte(WfSaw);
        chords->writeByte(SCflt1cut)->writeByte(100);   // flt
        chords->writeByte(SCflt1res)->writeByte(100);
        chords->writeByte(SCflt1mod)->writeByte(160);
        chords->writeByte(SCflt1mode)->writeByte(FmBandPass || FmHighPass);
    }
    var mono = NEW_(Stream);
    {
        mono->writeByte(43);
        mono->writeByte(SCamp)->writeByte(255);         // amp
        mono->writeByte(SCenv1amp)->writeFloat(1.0f);   // amp env
        mono->writeByte(SCenv1dc)->writeFloat(0.0f);
        mono->writeByte(SCenv1atk)->writeByte(  2);
        mono->writeByte(SCenv1dec)->writeByte( 38);
        mono->writeByte(SCenv1sus)->writeByte( 80);
        mono->writeByte(SCenv1rel)->writeByte( 42);
        mono->writeByte(SCenv2amp)->writeFloat(0.2f);   // psw env
        mono->writeByte(SCenv2dc)->writeFloat(0.45f);
        mono->writeByte(SCenv2atk)->writeByte(20);
        mono->writeByte(SCenv2dec)->writeByte(64);
        mono->writeByte(SCenv2sus)->writeByte(128);
        mono->writeByte(SCenv2rel)->writeByte(40);
        mono->writeByte(SCenv3amp)->writeFloat(1.0f);   // cut env
        mono->writeByte(SCenv3dc)->writeFloat(0.0f);
        mono->writeByte(SCenv3atk)->writeByte(2);
        mono->writeByte(SCenv3dec)->writeByte(32);
        mono->writeByte(SCenv3sus)->writeByte(80);
        mono->writeByte(SCenv3rel)->writeByte(42);
        mono->writeByte(SCenv4amp)->writeFloat(0.0f);   // fm env
        mono->writeByte(SCenv4dc)->writeFloat(0.0f);
        mono->writeByte(SCenv4atk)->writeByte(0);
        mono->writeByte(SCenv4dec)->writeByte(0);
        mono->writeByte(SCenv4sus)->writeByte(0);
        mono->writeByte(SCenv4rel)->writeByte(0);
        mono->writeByte(SClfo1amp)->writeFloat(0.2f);   // lfo1
        mono->writeByte(SClfo1fre)->writeFloat(2.2f);
        mono->writeByte(SClfo2amp)->writeFloat(1.0f);   // lfo2
        mono->writeByte(SClfo2fre)->writeFloat(1.8f);
        mono->writeByte(SCosc1amp)->writeFloat(0.6f);   // osc1
        mono->writeByte(SCosc1fre)->writeFloat(0.0f);
        mono->writeByte(SCosc1tune)->writeByte(0);
        mono->writeByte(SCosc1psw)->writeByte(0);
        mono->writeByte(SCosc1wave)->writeByte(WfSaw);
        mono->writeByte(SCosc2amp)->writeFloat(0.5f);   // osc2
        mono->writeByte(SCosc2fre)->writeFloat(0.1f);
        mono->writeByte(SCosc2tune)->writeByte(12);
        mono->writeByte(SCosc2psw)->writeByte(10);
        mono->writeByte(SCosc2wave)->writeByte(WfSaw);
        mono->writeByte(SCflt1cut)->writeByte(20);      // flt
        mono->writeByte(SCflt1res)->writeByte(120);
        mono->writeByte(SCflt1mod)->writeByte(140);
        mono->writeByte(SCflt1mode)->writeByte(FmLowPass);
    }

    int count = 6;
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

void SynthTest::testGenerateSoundSimple() {
    const int note1 = SynthPitch::pD5;
    const int note2 = SynthPitch::pFs4;
    const int note3 = SynthPitch::pA4;
    const int note4 = SynthPitch::pD2;
    //Mdl::createFrequencyTable();
    var synth = NEW_(Synth, &samplingRate, 6);
    byte* sb = NULL;
    var soundBank = createSoundBank();
    synth->soundBank(soundBank->data());
    synth->setProgram(4);

    if (SoundPlayer::start((int)samplingRate, 1, simpleSoundCallback, synth) == 0) {
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

    assert("Should create sound", synth->isActive);

    DEL_(synth);
    DEL_(soundBank);
}

void SynthTest::testGenerateSoundMixer() {
    const int note1 = SynthPitch::pFs3;
    const int note2 = SynthPitch::pA3;
    const int note3 = SynthPitch::pD4;
    const int note4 = SynthPitch::pD2;
    //Mdl::createFrequencyTable();
#pragma region Devices
    var synth1 = NEW_(Synth, &samplingRate, 1);
    synth1->setVoiceCount(6);
    var synth2 = NEW_(Synth, &samplingRate, 1);
    var soundBank = createSoundBank();
    synth1->soundBank(soundBank->data());
    synth1->setProgram(4);
    synth2->soundBank(soundBank->data());
    synth2->setProgram(0);
    var clip1 = NEW_(Distort, &samplingRate);
    var mixer = NEW_(Mixer8x4);
        mixer->channelCount(2);
#pragma endregion

    // channel 1
    mixer->connectInput(mixer->getChannel(0), synth1);
    mixer->getControl(MCch1gain)->value.f = 1.0f;
    mixer->getControl(MCch1pan)->value.f = 0.2f;
    mixer->getControl(MCch1amp)->value.f = 0.4f;

    // channel 2
    var ch2 = mixer->getChannel(1);
    mixer->connectInput(ch2, synth2);
    mixer->getControl(MCch2gain)->value.f = 0.4f;
    mixer->getControl(MCch2pan)->value.f = 0.7f;
    mixer->getControl(MCch2amp)->value.f = 0.8f;
        mixer->getControl(MCch2st1gain)->value.f = 0.5f;
        mixer->connectEffect(ch2, clip1);
        clip1->controls().amp.value.f = 2.5f;
        clip1->controls().lvl.value.f = 0.8f;
        clip1->controls().cut.value.b = 80;
        clip1->controls().res.value.f = 0.1f;
        clip1->controls().mode.value.b = FmLowPass || FmBandPass;
        clip1->updateFilter();


    if (SoundPlayer::start((int)samplingRate, 2, Mixer8x4::fillSoundBuffer, mixer) == 0) {
        for (var i = 0; i < 2; i++) {
            synth1->setNote(note1, 224);
            synth1->setNote(note2, 124);
            synth1->setNote(note3, 184);
            synth2->setNote(note4, 184);
            Sleep(2400);
            synth1->setNote(note1, 0);
            synth1->setNote(note2, 0);
            synth1->setNote(note3, 0);
            synth2->setNote(note4, 0);
            Sleep(1200);
        }
        Sleep(1000);
        SoundPlayer::stop();
    }

    assert("Should create sound", synth1->isActive);

    DEL_(synth1);
    DEL_(synth2);
    DEL_(clip1);
    DEL_(mixer);
    DEL_(soundBank);
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
    synthDevices[4]->synth()->setVoiceCount(6);
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
    var sequence3 = NEW_(Sequence, &synthDevice);
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
        sequence4->writeDelta(0)->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(180);sequence4->writeEOF();
        sequence4->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(  0); sequence4->writeEOF();

        sequence4->writeDelta(7)->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(220); sequence4->writeEOF();
        sequence4->writeDelta(4)->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(  0); sequence4->writeEOF();

        sequence4->writeDelta(4)->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(180); sequence4->writeEOF();
        sequence4->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(  0); sequence4->writeEOF();

        sequence4->writeDelta(7)->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(220); sequence4->writeEOF();
        sequence4->writeDelta(3)->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(  0); sequence4->writeEOF();

        sequence4->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(160); sequence4->writeEOF();
        sequence4->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(  0); sequence4->writeEOF();

        sequence4->writeDelta(6)->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(180); sequence4->writeEOF();
        sequence4->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(  0); sequence4->writeEOF();

        sequence4->writeDelta(3)->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(220); sequence4->writeEOF();
        sequence4->writeDelta(3)->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(  0); sequence4->writeEOF();

        sequence4->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(160); sequence4->writeEOF();
        sequence4->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(  0); sequence4->writeEOF();

        sequence4->writeDelta(3)->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(180); sequence4->writeEOF();
        sequence4->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(0  ); sequence4->writeEOF();

        sequence4->writeDelta(7)->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(220); sequence4->writeEOF();
        sequence4->writeDelta(3)->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(  0); sequence4->writeEOF();

        sequence4->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(170); sequence4->writeEOF();
        sequence4->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(  0); sequence4->writeEOF();

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
            adapterList->writeByte(0);  // program

        adapterList->writeByte(SynthDevices::DeviceSynth);  //dev #2
            adapterList->writeByte(1);  // voice count
            adapterList->writeByte(0);  // data block id for sound bank 
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
            adapterList->writeFloat(2.0f);      // amp
            adapterList->writeByte(120);        // lvl
            adapterList->writeByte( 20);        // cut
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
            adapterList->writeByte(100);        // feedbackl
            adapterList->writeFloat(380.5f);    // delayL
            adapterList->writeByte( 80);        // feedbackR
            adapterList->writeFloat(390.5f);    // delayR
            adapterList->writeByte(200);        // mixL
            adapterList->writeByte(100);        // mixR

        adapterList->writeByte(SynthDevices::DeviceMixer);  //dev #10
            adapterList->writeByte(5);
            
        // channel #1
            adapterList->writeByte(255);    // gain#1 - kick+snare
            adapterList->writeByte( 50);    // pan#1
            adapterList->writeByte(240);    // amp#1
            adapterList->writeByte(  0);    // stage count

        // channel #2
            adapterList->writeByte( 65);    // gain#2 - bass
            adapterList->writeByte( 60);    // pan#2
            adapterList->writeByte(120);    // amp#2
            adapterList->writeByte(1);      // stage count
                adapterList->writeByte(80);// stage gain

        // channel #3
            adapterList->writeByte(250);    // gain#3 - hihat
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
            adapterList->writeByte(160);    // gain#3 - mono
            adapterList->writeByte( 80);    // pan#5
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

    // header
    // 3 data blocks
    data->writeWord(4);
    data->writeDword(initData->length());
    data->writeDword(adapterList->length());
    data->writeDword(sequenceList->length());
    data->writeDword(soundBank->length());

    data->writeStream(initData);
    DEL_(initData);
    data->writeStream(adapterList);
    DEL_(adapterList);
    data->writeStream(sequenceList);
    DEL_(sequenceList);
    data->writeStream(soundBank);
    DEL_(soundBank);

    return data;
}

void SynthTest::testLoadBinary() {
    var bin = createBinaryData();
    Player::addAdapter(NEW_(SynthAdapter));
    var data = bin->extract();
    DEL_(bin);
    var device = PlayerDevice::create(&data);
    var player = device->player();
    assert("Should have 32.0 fps", player->refreshRate() == 32.0f);
    assert("Should have 11 devices", player->devices().length() == 11);
    assert("Should have PlayerDevice as #1", ((Device*)player->devices().getAt(0))->type() == PlayerDevices::DevicePlayer);
    assert("Should have SynthDevice as #2, #3, #4, #5 and #6",
        ((Device*)player->devices().getAt(1))->type() ==SynthDevices::DeviceSynth &&
        ((Device*)player->devices().getAt(2))->type() == SynthDevices::DeviceSynth &&
        ((Device*)player->devices().getAt(3))->type() == SynthDevices::DeviceSynth &&
        ((Device*)player->devices().getAt(4))->type() == SynthDevices::DeviceSynth &&
        ((Device*)player->devices().getAt(5))->type() == SynthDevices::DeviceSynth);
    assert("Should have ClipDevice as #7", ((Device*)player->devices().getAt(6))->type() == SynthDevices::DeviceClip);
    assert("Should have DelayDevice as #8, #9 and #10",
        ((Device*)player->devices().getAt(7))->type() == SynthDevices::DeviceClip &&
        ((Device*)player->devices().getAt(8))->type() == SynthDevices::DeviceClip &&
        ((Device*)player->devices().getAt(9))->type() == SynthDevices::DeviceClip);
    assert("Should have MixerDevice as #11", ((Device*)player->devices().getAt(10))->type() == SynthDevices::DeviceMixer);
    assert("Should have 6 sequences", player->sequences().length() == 6);
    assert("Should have 1 data-block", player->dataBlocks().length() == 1);
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
    int ix = 0;
    var mixer = (Mixer8x4*)((Device*)player->devices().search(NULL, ix,
        [](void* value, UINT32 ix, Collection* collection, void* key) {
            return ((Device*)value)->type() == SynthDevices::DeviceMixer ? 0 : 1;
        }
    ))->object();
    var samplesPerFrame = synthAdapter->samplingRate / player->refreshRate();
    int bufferSizeInSamples = 4096;
    int bufferSize = 2 * sizeof(short) * bufferSizeInSamples;
    var buffer = MALLOC(short, bufferSize);
    var offset = buffer;
    var wave = NEW_(WaveFmt, "ouput.wav", (int)samplingRate, 2, 16);
    var cons = getConsole();
    float frameCounter = 0;
    do {
        int remainingSamples = bufferSizeInSamples;
        memset(buffer, 0, bufferSize);
        var offset = buffer;
        while (remainingSamples > 0) {
            var count = remainingSamples > SAMPLE_BUFFER_SIZE ? SAMPLE_BUFFER_SIZE : remainingSamples;
            if (frameCounter <= 0) {
                player->run(1);
                frameCounter += samplesPerFrame;
            }
            if (frameCounter < count) {
                count = (int)frameCounter;
            }
            Mixer8x4::fillSoundBuffer(offset, count, mixer);
            offset += 2 * count;
            cons->printf(".");
            remainingSamples -= count;
            frameCounter -= count;
        }        
        wave->write((byte*)buffer, (int)(sizeof(short)*(offset - buffer)));
    } while (player->masterChannel()->isActive());

    wave->close();
    DEL_(wave);
    FREE(buffer);
    DEL_(device);
    Player::cleanUp();
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
    //tests->test("Load from binary", (TestMethod) & (SynthTest::testLoadBinary));
    //tests->test("Create binary", (TestMethod) & (SynthTest::testCreateBinary));
    //tests->test("Create sequence", (TestMethod) & (SynthTest::testCreateSequence));
    //tests->test("Playback sequence", (TestMethod) & (SynthTest::testPlaySequence));
    tests->test("Load from binary", (TestMethod) & (SynthTest::testLoadBinary));
    //tests->test("Save to wave", (TestMethod) & (SynthTest::testSaveToWave));
    //TEST(testSynthAdapter);
    //TEST(testLoadBinary);
    //TEST(testLoadXm);

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
//    player->masterChannel()->assign(0, (Sequence*)player->sequences().getAt(0));
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