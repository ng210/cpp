#include "consoleapp.h"
#include "synth/synth/synth.h"
#include "soundlib/src/soundplayer.h"
#include "soundlib/src/wavfmt.h"
#include "synth/synth/synth-controls.h"
#include "player/src/player.h"
#include "synth/synth/synth-adapter.h"
#include "test.h"
#include <synth/synth/synth-device.h>

NS_FW_BASE_USE
using namespace SYNTH;
using namespace PLAYER;

#define SamplingRate 48000

Synth* synth_ = NULL;

void soundCallback(short* buffer, int sampleCount, void* args) {
    synth_->run(buffer, 0, sampleCount);
}

typedef int RUN(void* args);

void run(RUN callback, void* args) {
    LARGE_INTEGER lastTime, currentTime;
    LARGE_INTEGER frequency;
    var player = (Player*)args;
    int quit = 0;
    SYSPR(QueryPerformanceCounter(&lastTime));
    SYSPR(QueryPerformanceFrequency(&frequency));
    qword countPerFrame = (qword)(frequency.QuadPart / player->refreshRate());
    qword counter = 0;
    while (!quit)
    {
        SYSPR(QueryPerformanceCounter(&currentTime));
        qword delta = (qword)(currentTime.QuadPart - lastTime.QuadPart);
        // check fps
        counter += delta;
        if (counter > countPerFrame) {
            counter -= countPerFrame;
            quit = callback(args);
        }
        lastTime = currentTime;
    }
}

size_t createSoundBank(byte*& sb) {
    Stream s(1024);
    s.writeByte(2);                         // 2 instruments
    s.writeString("synth01......");         // 1st label
    s.writeWord(1 + 2 * 16);                // 1st offset
    s.writeString("mono01.......");         // 2nd label
    s.writeWord(1 + 2 * 16 + 117);          // 2nd offset

    // synth01
    {
        s.writeByte(37);
        s.writeByte(SCamp)->writeByte(80);
        s.writeByte(SClfo1amp)->writeFloat(0.22f);
        s.writeByte(SClfo1fre)->writeFloat(5.00f);
        s.writeByte(SClfo2amp)->writeFloat(2.10f);
        s.writeByte(SClfo2fre)->writeFloat(5.20f);
        s.writeByte(SCenv1amp)->writeFloat(1.00f);
        s.writeByte(SCenv1dc) ->writeFloat(0.00f);
        s.writeByte(SCenv1atk)->writeByte(5);
        s.writeByte(SCenv1dec)->writeByte(25);
        s.writeByte(SCenv1sus)->writeByte(160);
        s.writeByte(SCenv1rel)->writeByte(44);
        s.writeByte(SCenv2amp)->writeFloat(0.40f);
        s.writeByte(SCenv2dc) ->writeFloat(0.00f);
        s.writeByte(SCenv2atk)->writeByte(51);
        s.writeByte(SCenv2dec)->writeByte(102);
        s.writeByte(SCenv2sus)->writeByte(180);
        s.writeByte(SCenv2rel)->writeByte(104);
        s.writeByte(SCenv3amp)->writeFloat(1.00f);
        s.writeByte(SCenv3dc) ->writeFloat(0.00f);
        s.writeByte(SCenv3atk)->writeByte(38);
        s.writeByte(SCenv3dec)->writeByte(102);
        s.writeByte(SCenv3sus)->writeByte(184);
        s.writeByte(SCenv3rel)->writeByte(41);
        s.writeByte(SCosc1amp) ->writeFloat(0.40f);
        s.writeByte(SCosc1fre) ->writeFloat(0.00f);
        s.writeByte(SCosc1tune)->writeByte(0);
        s.writeByte(SCosc1psw) ->writeByte(0);
        s.writeByte(SCosc1wave)->writeByte(WfPulse);
        s.writeByte(SCosc2amp) ->writeFloat(0.50f);
        s.writeByte(SCosc2fre) ->writeFloat(1.00f);
        s.writeByte(SCosc2tune)->writeByte(12);
        s.writeByte(SCosc2psw) ->writeByte(30);
        s.writeByte(SCosc2wave)->writeByte(WfPulse);
        s.writeByte(SCflt1cut) ->writeByte(20);
        s.writeByte(SCflt1res) ->writeByte(124);
        s.writeByte(SCflt1mod) ->writeByte(20);
        s.writeByte(SCflt1mode)->writeByte(FmLowPass);
    }

    // mono01
    {
        s.writeByte(31);
        s.writeByte(SCamp)->writeByte(40);
        s.writeByte(SCenv1amp)->writeFloat(1.0f);
        s.writeByte(SCenv2amp)->writeFloat(0.4f);
        s.writeByte(SCenv3amp)->writeFloat(1.0f);
        s.writeByte(SCenv1dc)->writeFloat(0.0f);
        s.writeByte(SCenv2dc)->writeFloat(0.3f);
        s.writeByte(SCenv3dc)->writeFloat(0.0f);
        s.writeByte(SCenv1atk)->writeByte(0);
        s.writeByte(SCenv2atk)->writeByte(0);
        s.writeByte(SCenv3atk)->writeByte(0);
        s.writeByte(SCenv1dec)->writeByte(20);
        s.writeByte(SCenv2dec)->writeByte(20);
        s.writeByte(SCenv3dec)->writeByte(10);
        s.writeByte(SCenv1sus)->writeByte(60);
        s.writeByte(SCenv2sus)->writeByte(20);
        s.writeByte(SCenv3sus)->writeByte(10);
        s.writeByte(SCenv1rel)->writeByte(8);
        s.writeByte(SCenv2rel)->writeByte(10);
        s.writeByte(SCenv3rel)->writeByte(10);
        s.writeByte(SCosc1amp)->writeFloat(0.4f);
        s.writeByte(SCosc2amp)->writeFloat(0.2f);
        s.writeByte(SCosc1fre)->writeFloat(0.0f);
        s.writeByte(SCosc2fre)->writeFloat(0.02f);
        s.writeByte(SCosc1tune)->writeByte(0);
        s.writeByte(SCosc2tune)->writeByte(12);
        s.writeByte(SCosc1wave)->writeByte(WfSaw);
        s.writeByte(SCosc2wave)->writeByte(WfSaw);
        s.writeByte(SCflt1cut)->writeByte(10);
        s.writeByte(SCflt1res)->writeByte(120);
        s.writeByte(SCflt1mod)->writeByte(80);
        s.writeByte(SCflt1mode)->writeByte(FmLowPass);
    }
    var len = s.cursor() - s.data();

    sb = REALLOC(sb, byte, len);
    fmw::memcpy(sb, s.data(), len);
    return len;
}

int testGenerateSound();
int testSynthAdapter();

int _main(NS_FW_BASE::Map* args) {
#ifdef _DEBUG
    Debug::initialize(/*DEBUG_UNICODE | DEBUG_MEMORY*/);
#endif

    LOG("Player tests\n");
    int passed = 0, failed = 0;

    //TEST(testGenerateSound);
    TEST(testSynthAdapter);

    printf("*** Final results: %d/%d=%.02f%%\n\n", passed, (passed + failed), (100.0f * passed) / (passed + failed));

    return 0;
}

int testGenerateSound() {
    HEADER("Test generate sound");

    Mdl::createFrequencyTable();
    synth_ = NEW_(Synth, SamplingRate, 3);
    byte* sb = NULL;
    var len = createSoundBank(sb);
    synth_->soundBank(sb);
    synth_->setProgram(0);
    synth_->isActive = true;
    synth_->setNote(21, 224);
    synth_->setNote(54, 184);
    synth_->setNote(50, 124);

    if (SoundPlayer::start(SamplingRate, 1, soundCallback) == 0) {
        Sleep(4000);
        SoundPlayer::stop();
    }

    //var totalLength = SamplingRate * 4; // 4 secs
    //var buffer = MALLOC(short, totalLength);
    //synth_->run(buffer, 0, totalLength);
    //WaveFmt::write("out1.wav", SamplingRate, 1, 16, (byte*)buffer, 2 * totalLength);
    //FREE(buffer);

    DEL_(synth_);
    FREE(sb);

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
    WRITE(p, 2, byte);          // 2 device
    WRITE(p, DevSynth, byte);   // #1 synth
    WRITE(p, 3, byte);          // voice count
    WRITE(p, 2, byte);          // soundbank id
    WRITE(p, DevSynth, byte);   // #2 synth
    WRITE(p, 1, byte);          // voice count
    WRITE(p, 2, byte);          // soundbank id

    byte* sb = NULL;
    var len = (int)createSoundBank(sb);

    // add adapters
    Player::registerAdapter(SynthAdapter::Info);
    var player = (Player*)Player::creator(NULL);
    player->prepareContext(db1);
    player->addDatablock(db1, 3);
    player->addDatablock(db2, 9);
    player->addDatablock(sb, len);
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
    run([](void* args) {
        var p = (Player*)args;
        return p->run(1) ? 0 : 1;
        }, player);

    SoundPlayer::stop();

    DEL_(player);

    RESULTS();
    return failed;
}
