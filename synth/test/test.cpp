#include "test.h"
#include "consoleapp.h"
#include "soundlib/src/soundplayer.h"
#include "soundlib/src/wavfmt.h"
#include "player/src/player.h"
#include "synth/src/synth.h"
#include "synth/src/synth-controls.h"
#include "synth/src/synth-adapter.h"
#include "synth/src/synth-device.h"
#include "synth/src/xmloader.h"

NS_FW_BASE_USE
using namespace SYNTH;
using namespace PLAYER;

#define SamplingRate 48000

Synth* synth_ = NULL;
double framePerSecond_ = 0.0;

void simpleSoundCallback(short* buffer, int sampleCount, void* args) {
    synth_->run(buffer, 0, sampleCount);
}

void playerBasedSoundCallback(short* buffer, int sampleCount, void* args) {
    var ad = (Adapter*)args;
    for (var i = 0; i < ad->devices().length(); i++) {
        var dev = (SynthDevice*)ad->getDevice(i);
        dev->run(buffer, 0, sampleCount);
    }
}

typedef int RUN(void* args, int& loopCount);

void run(RUN callback, void* args, int loopCount) {
    LARGE_INTEGER lastTime, currentTime;
    LARGE_INTEGER frequency;
    var player = (Player*)args;
    int isRunning = 1;
    SYSPR(QueryPerformanceCounter(&lastTime));
    SYSPR(QueryPerformanceFrequency(&frequency));
    qword countPerFrame = (qword)(frequency.QuadPart / player->refreshRate());
    qword counter = 0;
    while (isRunning)
    {
        SYSPR(QueryPerformanceCounter(&currentTime));
        qword delta = (qword)(currentTime.QuadPart - lastTime.QuadPart);
        // check fps
        counter += delta;
        while (counter > countPerFrame) {
            //printf("%lld, ", counter);
            counter -= countPerFrame;
            isRunning = callback(args, loopCount);
        }
        lastTime = currentTime;
    }
}

Stream* createSoundBank() {
    Stream* s = NEW_(Stream, 128);
    s->writeByte(4);                        // 3 instruments
    s->writeString("bassline.....");        // 1st label
    s->writeWord(1 + 4 * 16 + 0);           // 1st offset
    s->writeString("kick.........");        // 2nd label
    s->writeWord(1 + 4 * 16 + 140);         // 2nd offset
    s->writeString("snare........");        // 3rd label
    s->writeWord(1 + 4 * 16 + 280);         // 3rd offset
    s->writeString("chords.......");        // 4th label
    s->writeWord(1 + 4 * 16 + 420);         // 4th offset

    // bassline
    {
        s->writeByte(44);
        s->writeByte(SCamp)->writeByte(60);
        
        s->writeByte(SCenv1amp)->writeFloat(1.0f)->writeByte(SCenv1dc)->writeFloat(0.0f);
        s->writeByte(SCenv1atk)->writeByte(0);
        s->writeByte(SCenv1dec)->writeByte(10);
        s->writeByte(SCenv1sus)->writeByte(160);
        s->writeByte(SCenv1rel)->writeByte(10);
        
        s->writeByte(SCenv2amp)->writeFloat(0.01f)->writeByte(SCenv2dc)->writeFloat(0.5f);
        s->writeByte(SCenv2atk)->writeByte(0);
        s->writeByte(SCenv2dec)->writeByte(10);
        s->writeByte(SCenv2sus)->writeByte(128);
        s->writeByte(SCenv2rel)->writeByte(40);
        
        s->writeByte(SCenv3amp)->writeFloat(1.0f)->writeByte(SCenv3dc)->writeFloat(0.0f);
        s->writeByte(SCenv3atk)->writeByte(2);
        s->writeByte(SCenv3dec)->writeByte(6);
        s->writeByte(SCenv3sus)->writeByte(20);
        s->writeByte(SCenv3rel)->writeByte(40);
        
        s->writeByte(SCenv4amp)->writeFloat(0.0f)->writeByte(SCenv4dc)->writeFloat(0.0f);
        s->writeByte(SCenv4atk)->writeByte(0);
        s->writeByte(SCenv4dec)->writeByte(0);
        s->writeByte(SCenv4sus)->writeByte(0);
        s->writeByte(SCenv4rel)->writeByte(0);
        
        s->writeByte(SClfo1amp)->writeFloat(0.0f)->writeByte(SClfo1fre)->writeFloat(3.1f);
        s->writeByte(SClfo2amp)->writeFloat(0.0f)->writeByte(SClfo2fre)->writeFloat(3.8f);
        
        s->writeByte(SCosc1amp)->writeFloat(0.4f);
        s->writeByte(SCosc1fre)->writeFloat(0.0f);
        //s->writeByte(SCosc1note)->writeByte(0);
        s->writeByte(SCosc1tune)->writeByte(12);
        s->writeByte(SCosc1psw)->writeByte(0);
        s->writeByte(SCosc1wave)->writeByte(WfPulse);
        
        s->writeByte(SCosc2amp)->writeFloat(0.1f);
        s->writeByte(SCosc2fre)->writeFloat(0.03f);
        //s->writeByte(SCosc2note)->writeByte(0);
        s->writeByte(SCosc2tune)->writeByte(24);
        s->writeByte(SCosc2psw)->writeByte(0);
        s->writeByte(SCosc2wave)->writeByte(WfPulse);
        
        s->writeByte(SCflt1amp)->writeFloat(0.0f);
        s->writeByte(SCflt1cut)->writeByte(60);
        s->writeByte(SCflt1res)->writeByte(20);
        s->writeByte(SCflt1mod)->writeByte(40);
        s->writeByte(SCflt1mode)->writeByte(FmLowPass);
    }

    // kick
    {
        s->writeByte(44);
        s->writeByte(SCamp)->writeByte(80);
        
        s->writeByte(SCenv1amp)->writeFloat(1.0f)->writeByte(SCenv1dc)->writeFloat(0.0f);
        s->writeByte(SCenv1atk)->writeByte(0);
        s->writeByte(SCenv1dec)->writeByte(4);
        s->writeByte(SCenv1sus)->writeByte(120);
        s->writeByte(SCenv1rel)->writeByte(4);
        
        s->writeByte(SCenv2amp)->writeFloat(0.0f)->writeByte(SCenv2dc)->writeFloat(0.0f);
        s->writeByte(SCenv2atk)->writeByte(0);
        s->writeByte(SCenv2dec)->writeByte(0);
        s->writeByte(SCenv2sus)->writeByte(0);
        s->writeByte(SCenv2rel)->writeByte(0);
        
        s->writeByte(SCenv3amp)->writeFloat(1.0f)->writeByte(SCenv3dc)->writeFloat(0.0f);
        s->writeByte(SCenv3atk)->writeByte(0);
        s->writeByte(SCenv3dec)->writeByte(0);
        s->writeByte(SCenv3sus)->writeByte(1);
        s->writeByte(SCenv3rel)->writeByte(10);
        
        s->writeByte(SCenv4amp)->writeFloat(220.0f)->writeByte(SCenv4dc)->writeFloat(14.0f);
        s->writeByte(SCenv4atk)->writeByte(0);
        s->writeByte(SCenv4dec)->writeByte(4);
        s->writeByte(SCenv4sus)->writeByte(20);
        s->writeByte(SCenv4rel)->writeByte(10);
        
        s->writeByte(SClfo1amp)->writeFloat(0.0f)->writeByte(SClfo1fre)->writeFloat(0.0f);
        s->writeByte(SClfo2amp)->writeFloat(0.0f)->writeByte(SClfo2fre)->writeFloat(0.0f);
        
        s->writeByte(SCosc1amp)->writeFloat(0.9f);
        s->writeByte(SCosc1fre)->writeFloat(0.0f);
        //s->writeByte(SCosc1note)->writeByte(0);
        s->writeByte(SCosc1tune)->writeByte(0);
        s->writeByte(SCosc1psw)->writeByte(0);
        s->writeByte(SCosc1wave)->writeByte(WfSinus);
        
        s->writeByte(SCosc2amp)->writeFloat(0.03f);
        s->writeByte(SCosc2fre)->writeFloat(6120.0f);
        //s->writeByte(SCosc2note)->writeByte(0);
        s->writeByte(SCosc2tune)->writeByte(0);
        s->writeByte(SCosc2psw)->writeByte(0);
        s->writeByte(SCosc2wave)->writeByte(WfNoise);
        
        s->writeByte(SCflt1amp)->writeFloat(1.0f);
        s->writeByte(SCflt1cut)->writeByte(4);
        s->writeByte(SCflt1res)->writeByte(120);
        s->writeByte(SCflt1mod)->writeByte(160);
        s->writeByte(SCflt1mode)->writeByte(FmLowPass);
    }

    // snare
    {
        s->writeByte(44);
        s->writeByte(SCamp)->writeByte(80);

        s->writeByte(SCenv1amp)->writeFloat(1.0f)->writeByte(SCenv1dc)->writeFloat(0.0f);
        s->writeByte(SCenv1atk)->writeByte(0);
        s->writeByte(SCenv1dec)->writeByte(4);
        s->writeByte(SCenv1sus)->writeByte(20);
        s->writeByte(SCenv1rel)->writeByte(3);

        s->writeByte(SCenv2amp)->writeFloat(0.0f)->writeByte(SCenv2dc)->writeFloat(0.0f);
        s->writeByte(SCenv2atk)->writeByte(0);
        s->writeByte(SCenv2dec)->writeByte(0);
        s->writeByte(SCenv2sus)->writeByte(0);
        s->writeByte(SCenv2rel)->writeByte(0);

        s->writeByte(SCenv3amp)->writeFloat(1.0f)->writeByte(SCenv3dc)->writeFloat(0.0f);
        s->writeByte(SCenv3atk)->writeByte(0);
        s->writeByte(SCenv3dec)->writeByte(2);
        s->writeByte(SCenv3sus)->writeByte(120);
        s->writeByte(SCenv3rel)->writeByte(10);

        s->writeByte(SCenv4amp)->writeFloat(20.0f)->writeByte(SCenv4dc)->writeFloat(160.0f);
        s->writeByte(SCenv4atk)->writeByte(0);
        s->writeByte(SCenv4dec)->writeByte(2);
        s->writeByte(SCenv4sus)->writeByte(20);
        s->writeByte(SCenv4rel)->writeByte(40);

        s->writeByte(SClfo1amp)->writeFloat(0.2f)->writeByte(SClfo1fre)->writeFloat(16.2f);
        s->writeByte(SClfo2amp)->writeFloat(16.1f)->writeByte(SClfo2fre)->writeFloat(71.3f);

        s->writeByte(SCosc1amp)->writeFloat(0.67f);
        s->writeByte(SCosc1fre)->writeFloat(0.0f);
        //s->writeByte(SCosc1note)->writeByte(0);
        s->writeByte(SCosc1tune)->writeByte(0);
        s->writeByte(SCosc1psw)->writeByte(0);
        s->writeByte(SCosc1wave)->writeByte(WfPulse);

        s->writeByte(SCosc2amp)->writeFloat(0.41f);
        s->writeByte(SCosc2fre)->writeFloat(6163.63f);
        //s->writeByte(SCosc2note)->writeByte(0);
        s->writeByte(SCosc2tune)->writeByte(0);
        s->writeByte(SCosc2psw)->writeByte(0);
        s->writeByte(SCosc2wave)->writeByte(WfNoise);

        s->writeByte(SCflt1amp)->writeFloat(1.0f);
        s->writeByte(SCflt1cut)->writeByte(60);
        s->writeByte(SCflt1res)->writeByte(0);
        s->writeByte(SCflt1mod)->writeByte(80);
        s->writeByte(SCflt1mode)->writeByte(FmBandPass);
    }

    // chords
    {
        s->writeByte(44);
        s->writeByte(SCamp)->writeByte(60);

        s->writeByte(SCenv1amp)->writeFloat(1.0f)->writeByte(SCenv1dc)->writeFloat(0.0f);
        s->writeByte(SCenv1atk)->writeByte(1);
        s->writeByte(SCenv1dec)->writeByte(4);
        s->writeByte(SCenv1sus)->writeByte(40);
        s->writeByte(SCenv1rel)->writeByte(20);

        s->writeByte(SCenv2amp)->writeFloat(0.06f)->writeByte(SCenv2dc)->writeFloat(0.5f);
        s->writeByte(SCenv2atk)->writeByte(1);
        s->writeByte(SCenv2dec)->writeByte(6);
        s->writeByte(SCenv2sus)->writeByte(120);
        s->writeByte(SCenv2rel)->writeByte(40);

        s->writeByte(SCenv3amp)->writeFloat(1.0f)->writeByte(SCenv3dc)->writeFloat(0.0f);
        s->writeByte(SCenv3atk)->writeByte(0);
        s->writeByte(SCenv3dec)->writeByte(6);
        s->writeByte(SCenv3sus)->writeByte(20);
        s->writeByte(SCenv3rel)->writeByte(40);

        s->writeByte(SCenv4amp)->writeFloat(0.0f)->writeByte(SCenv4dc)->writeFloat(0.0f);
        s->writeByte(SCenv4atk)->writeByte(0);
        s->writeByte(SCenv4dec)->writeByte(0);
        s->writeByte(SCenv4sus)->writeByte(0);
        s->writeByte(SCenv4rel)->writeByte(0);

        s->writeByte(SClfo1amp)->writeFloat(0.2f)->writeByte(SClfo1fre)->writeFloat(3.1f);
        s->writeByte(SClfo2amp)->writeFloat(6.2f)->writeByte(SClfo2fre)->writeFloat(3.8f);

        s->writeByte(SCosc1amp)->writeFloat(0.5f);
        s->writeByte(SCosc1fre)->writeFloat(0.0f);
        //s->writeByte(SCosc1note)->writeByte(0);
        s->writeByte(SCosc1tune)->writeByte(12);
        s->writeByte(SCosc1psw)->writeByte(0);
        s->writeByte(SCosc1wave)->writeByte(WfSaw);

        s->writeByte(SCosc2amp)->writeFloat(0.2f);
        s->writeByte(SCosc2fre)->writeFloat(0.1f);
        //s->writeByte(SCosc2note)->writeByte(0);
        s->writeByte(SCosc2tune)->writeByte(24);
        s->writeByte(SCosc2psw)->writeByte(0);
        s->writeByte(SCosc2wave)->writeByte(WfSaw);

        s->writeByte(SCflt1amp)->writeFloat(0.0f);
        s->writeByte(SCflt1cut)->writeByte(30);
        s->writeByte(SCflt1res)->writeByte(20);
        s->writeByte(SCflt1mod)->writeByte(40);
        s->writeByte(SCflt1mode)->writeByte(FmBandPass);
    }

    return s;
}

int testGenerateSound();
int testSynthAdapter();
int testLoadBinary();
int testSaveToWave();
int testLoadXm();

int _main(NS_FW_BASE::Map* args) {
#ifdef _DEBUG
    Debug::initialize(/*DEBUG_UNICODE | DEBUG_MEMORY*/);
#endif

    LOG("Player tests\n");
    int passed = 0, failed = 0;

    //TEST(testGenerateSound);
    //TEST(testSynthAdapter);
    //TEST(testLoadBinary);
    //TEST(testSaveToWave);
    TEST(testLoadXm);

    printf("*** Final results: %d/%d=%.02f%%\n\n", passed, (passed + failed), (100.0f * passed) / (passed + failed));

    return 0;
}

void playback(Player* player, int loopCount) {
    // MONO playback
    var adapter = player->getAdapter(SynthAdapter::Info.id);
    SoundPlayer::start(SamplingRate, 1, playerBasedSoundCallback, adapter);
    run([](void* args, int& loopCount) {
        var p = (Player*)args;
        if (!p->run(1)) {
            p->reset();
            p->run(0);
            loopCount--;
        }
        return loopCount;
        }, player, loopCount);
    SoundPlayer::stop();
}

void saveToWave(char* path, Player* player) {
    // MONO
    var adapter = player->getAdapter(SynthAdapter::Info.id);
    float samplePerFrame = SamplingRate / player->refreshRate();
    int bufferSize = sizeof(short) * (int)samplePerFrame;
    var buffer = MALLOC(short, samplePerFrame);
    var wave = new WaveFmt(path, SamplingRate, 1, 16);
    while (player->run(1)) {
        memset(buffer, 0, bufferSize);
        playerBasedSoundCallback(buffer, (int)samplePerFrame, adapter);
        printf(".");
        wave->write((byte*)buffer, bufferSize);
    }
    wave->close();

    printf("\nWrite to %s\n", path);

    FREE(buffer);
}

int testGenerateSound() {
    HEADER("Test generate sound");

    //Mdl::createFrequencyTable();
    synth_ = NEW_(Synth, SamplingRate, 1);
    byte* sb = NULL;
    var soundBank = createSoundBank();
    synth_->soundBank(soundBank->data());
    synth_->setProgram(0);
    synth_->setNote(24, 124);

    if (SoundPlayer::start(SamplingRate, 1, simpleSoundCallback) == 0) {
        Sleep(2000);
        SoundPlayer::stop();
    }

    //var totalLength = SamplingRate * 4; // 4 secs
    //var buffer = MALLOC(short, totalLength);
    //synth_->run(buffer, 0, totalLength);
    //WaveFmt::write("out1.wav", SamplingRate, 1, 16, (byte*)buffer, 2 * totalLength);
    //FREE(buffer);

    DEL_(synth_);
    DEL_(soundBank);

    RESULTS();
    return failed;
}

int testSynthAdapter() {
    HEADER("Test SynthAdapter");

    // create user data blocks
    byte* db1 = MALLOC(byte, 3);
    db1[0] = 2; db1[1] = DevChannel; db1[2] = DevChannel;
    byte* db2 = MALLOC(byte, 9);
    var p = db2;
    WRITE(p, 48000, word);      // sampling rate
    WRITE(p, 2, byte);          // 3 device
    WRITE(p, DevSynth, byte);   // #1 synth
    WRITE(p, 3, byte);          // voice count
    WRITE(p, 2, byte);          // data block id of soundbank
    WRITE(p, DevSynth, byte);   // #2 synth
    WRITE(p, 1, byte);          // voice count
    WRITE(p, 2, byte);          // data block id of soundbank
    WRITE(p, DevSynth, byte);   // #3 synth
    WRITE(p, 1, byte);          // voice count
    WRITE(p, 2, byte);          // data block id of soundbank


    var sb = createSoundBank();

    // add adapters
    Player::registerAdapter(SynthAdapter::Info);
    var player = (Player*)Player::creator(NULL);
    player->prepareContext(db1);
    player->addDatablock(db1, 3);
    player->addDatablock(db2, 9);
    player->addDatablock(sb->extract(), sb->length());
    var adapter = player->addAdapter(SynthAdapter::Info.id, 1);
    adapter->prepareContext(db2);

    var pcount = 16;
    //#region MASTER sequence - 12
    {
        var sequence = NEW_(Sequence, player);
        sequence->writeHeader();
        // Frame #1 - 8
        sequence->writeDelta(0);
        sequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(0)->writeByte(2*pcount);
        sequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(2)->writeByte(2)->writeByte(1)->writeByte(pcount/4);
        sequence->writeEOF();
        // Frame #2 - 3
        sequence->writeDelta(pcount*64);
        sequence->writeEOS();
        player->sequences().add(sequence);
    }
    //#region Sequence #1 - 
    {
        var sequence = NEW_(Sequence, adapter);
        sequence->writeHeader();
        // Frame #1.1 - 8 bytes
        sequence->writeDelta(0);
        sequence->writeCommand(CmdSetProgram)->writeByte(1);
        sequence->writeCommand(CmdSetNote)->writeByte(pC3)->writeByte(128);
        sequence->writeEOF();
        // Frame #1.2 - 6 bytes
        sequence->writeDelta(4);
        sequence->writeCommand(CmdSetNote)->writeByte(pC3)->writeByte(0);
        sequence->writeEOF();

        // Frame #2.1 - 6 bytes
        sequence->writeDelta(4);
        sequence->writeCommand(CmdSetNote)->writeByte(pC3)->writeByte(100);
        sequence->writeEOF();
        // Frame #2.2 - 6 bytes
        sequence->writeDelta(4);
        sequence->writeCommand(CmdSetNote)->writeByte(pC3)->writeByte(0);
        sequence->writeEOF();

        // Frame #3.1 - 6 bytes
        sequence->writeDelta(4);
        sequence->writeCommand(CmdSetNote)->writeByte(pDs3)->writeByte(128);
        sequence->writeEOF();
        // Frame #3.2 - 6 bytes
        sequence->writeDelta(4);
        sequence->writeCommand(CmdSetNote)->writeByte(pDs3)->writeByte(0);
        sequence->writeEOF();

        // Frame #4.1 - 6 bytes
        sequence->writeDelta(4);
        sequence->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(140);
        sequence->writeEOF();
        // Frame #4.2 - 6 bytes
        sequence->writeDelta(4);
        sequence->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(0);
        sequence->writeEOF();
        // Frame end
        sequence->writeDelta(4);
        sequence->writeEOS();
        player->sequences().add(sequence);
    }

    //#region Sequence #2 - 
    {
        var sequence = NEW_(Sequence, adapter);
        sequence->writeHeader();
        // Frame #1.1
        sequence->writeDelta(0);
        sequence->writeCommand(CmdSetProgram)->writeByte(0);
        sequence->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(128);
        sequence->writeEOF();
        // Frame #1.2
        sequence->writeDelta(32);
        sequence->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(0);
        sequence->writeEOF();

        // Frame #2.1
        sequence->writeDelta(32);
        sequence->writeCommand(CmdSetNote)->writeByte(pAs1)->writeByte(128);
        sequence->writeEOF();
        // Frame #2.2
        sequence->writeDelta(32);
        sequence->writeCommand(CmdSetNote)->writeByte(pAs1)->writeByte(0);
        sequence->writeEOF();

        // Frame #3.1
        sequence->writeDelta(32);
        sequence->writeCommand(CmdSetNote)->writeByte(pF1)->writeByte(128);
        sequence->writeEOF();
        // Frame #3.2
        sequence->writeDelta(32);
        sequence->writeCommand(CmdSetNote)->writeByte(pF1)->writeByte(0);
        sequence->writeEOF();

        // Frame #3.3
        sequence->writeDelta(16);
        sequence->writeCommand(CmdSetNote)->writeByte(pG1)->writeByte(128);
        sequence->writeEOF();
        // Frame #3.4
        sequence->writeDelta(8);
        sequence->writeCommand(CmdSetNote)->writeByte(pG1)->writeByte(0);
        sequence->writeEOF();

        // Frame #4.1
        sequence->writeDelta(8);
        sequence->writeCommand(CmdSetNote)->writeByte(pGs1)->writeByte(128);
        sequence->writeEOF();
        // Frame #4.2
        sequence->writeDelta(16);
        sequence->writeCommand(CmdSetNote)->writeByte(pGs1)->writeByte(0);
        sequence->writeEOF();

        // Frame #4.3
        sequence->writeDelta(16);
        sequence->writeCommand(CmdSetNote)->writeByte(pAs1)->writeByte(128);
        sequence->writeEOF();
        // Frame #4.4
        sequence->writeDelta(16);
        sequence->writeCommand(CmdSetNote)->writeByte(pAs1)->writeByte(0);
        sequence->writeEOF();

        sequence->writeDelta(16);
        sequence->writeEOS();
        player->sequences().add(sequence);
    }

    //ASSERT("Should ", );

    // assign master channel
    player->masterChannel()->assign(0, (Sequence*)player->sequences().getAt(0));

    //Mdl::createFrequencyTable();
    SoundPlayer::start(SamplingRate, 1,
        [](short* buffer, int sampleCount, void* args) {
            for (var i = 0; i < sampleCount; i++) {
                buffer[i] = 0;
            }

            var ad = (Adapter*)args;
            for (var i = 0; i < ad->devices().length(); i++) {
                var dev = (SynthDevice*)ad->getDevice(i);
                dev->run(buffer, 0, sampleCount);
            }
        }, adapter);

    player->setRefreshRate(117.0f/3.75f);
    run([](void* args, int& loopCount) {
        var p = (Player*)args;
        return p->run(1) ? 0 : 1;
        }, player, 0);

    SoundPlayer::stop();

    DEL_(player);

    RESULTS();
    return failed;
}

int testLoadBinary() {
    HEADER("Load binary");
    const float bpm = 127.0f/3.75f;

    Player::registerAdapter(SynthAdapter::Info);
    var player = (Player*)Player::creator(NULL);
    var bytesRead = player->load("drums1.bin"); 
    player->setRefreshRate(bpm);
    var adapter = player->getAdapter(SynthAdapter::Info.id);
    //framePerSecond_ = bpm*3.75;
    
    playback(player, 1);

    //SoundPlayer::start(SamplingRate, 1, playerBasedSoundCallback, adapter);

    //run([](void* args) {
    //    var p = (Player*)args;
    //    if (!p->run(1)) {
    //        p->reset();
    //        p->run(0);
    //    }
    //    return 0;
    //    }, player);

    //SoundPlayer::stop();

    DEL_(player);

    RESULTS();
    return failed;
}

int testSaveToWave() {
    HEADER("Test save to wave");
    Player::registerAdapter(SynthAdapter::Info);
    var player = (Player*)Player::creator(NULL);
    var bytesRead = player->load("drums1.bin");
    const float bpm = 120.0f/3.75f;
    player->setRefreshRate(bpm);
    var adapter = (SynthAdapter*)player->getAdapter(SynthAdapter::Info.id);

    //playback(player);

    saveToWave("test1.wav", player);

    DEL_(player);
    return failed;
}

Player* loadXm(char* path) {
    Player::registerAdapter(SynthAdapter::Info);
    var player = (Player*)Player::creator(NULL);
    var adapter = player->addAdapter(SynthAdapter::Info.id, 1);
    adapter->player(player);
    var soundBank = createSoundBank();
    var xmLoader = NEW_(XmLoader, player, soundBank);
    var error = xmLoader->load(path);
    if (error != 0) {
        printf("Error loading xm (%d)", error);
    }
    else {
#ifdef _DEBUG
        for (var i = 0; i < xmLoader->patterns().length(); i++) {
            var buffer = xmLoader->printPattern(i);
            printf("Pattern #%d\n%s", (i+1), buffer);
            FREE(buffer);
        }
#endif
    }

    player->sequences().forEach([](void* item, UINT32 ix, Collection* col, void* args) {
        var seq = (Sequence*)item;
        var text = seq->print();
        printf("Seq #%d\n%s", ix+1, text);
        FREE(text);
        return 1;
    });

    DEL_(xmLoader);

    ((SynthDevice*)adapter->getDevice(0))->synth()->setVoiceCount(2);
    ((SynthDevice*)adapter->getDevice(3))->synth()->setVoiceCount(3);

    player->masterChannel()->assign(0, (Sequence*)player->sequences().getAt(0));

    return player;
}

int testLoadXm() {
    const char* fileName = "chords";
    var player = loadXm(str_format("%s.xm", fileName));
    const float bpm = 1.5f * 127.0f / 3.75f;
    player->setRefreshRate(bpm);
    saveToWave(str_format("%s.wav", fileName), player);
    player->masterChannel()->loopCount(2);
    player->masterChannel()->reset();
    playback(player, 2);
    DEL_(player);
    return 0;
}