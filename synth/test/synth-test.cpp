#include <math.h>
#include "synth/test/synth-test.h"
#include "base/memory.h"
#include "base/debug.h"
#include "synth/src/device/bass-device-ext.h"

// remove when drums is included
#include "synth/src/elem/dahr.h"

void SynthTest::simpleSynthCallback(short* buffer, int sampleCount, void* context) {
    //((Synth*)context)->run(buffer, 0, sampleCount);
    //for (var i = 0; i < sampleCount; i++) {
    //    SynthTest::buffer_[i] = 0.0f;
    //}
    var mdl = (Module*)context;
    mdl->run(0, sampleCount);
    for (var i = 0; i < sampleCount; i++) {
        buffer[i] = (short)(32768.0f * mdl->getOutput(0)[i]);
    }
}

void SynthTest::playback(Module* mdl, PlayCallback callback, float fps, char* path) {
    int msPerFrame = (int)(1000.0f / fps);
    int frame = 0;
    bool hasError = false;
    double frameSize = samplingRate / fps;

    if (path != NULL && saveToFile) {
        // prepare wave file and buffer
        var wave = NEW_(WaveFmt, path, (int)samplingRate, 1, 16);
        int length = (int)frameSize * sizeof(short);
        var buffer = MALLOC(short, length);
        memset(buffer, 0, length);
        wave->write((byte*)buffer, SAMPLE_BUFFER_SIZE);
        var fileLength = 0;

        // render sound into buffer
        int fi = 0;
        while (callback(mdl, fi++)) {
            LOG(".");
            // render 1 frame into the buffer
            var output = mdl->getOutput(0);
            var p = buffer;
            var len = length>>1;
            while (len > 0) {
                var toWrite = len > SAMPLE_BUFFER_SIZE ? SAMPLE_BUFFER_SIZE : len;
                mdl->run(0, toWrite);
                for (var j = 0; j < toWrite; j++) {
                    *p++ = (short)(32767.0f * output[j]);
                }
                len -= toWrite;
            }
            var byteCount = 2 * (int)(p - buffer);
            wave->write((byte*)buffer, byteCount);
            fileLength += byteCount;
        }
        wave->close();
        DEL_(wave);
        FREE(buffer);
        LOG("\nSaved %d bytes into '%s'.\n", fileLength, path);
    }
    else {
        if (SoundPlayer::start((int)samplingRate, 1, SynthTest::simpleSynthCallback, mdl) == 0) {
            int fi = 0;
            while (callback(mdl, fi++)) {
                LOG(".");
                Sleep(msPerFrame);
            }
            Sleep(1000);
            SoundPlayer::stop();
            LOG("\nPlayback stopped.\n");
        }
        else {
            LOG("Sound playback error!");
        }
    }
}

int SynthTest::testEnvelope(Env& env, short* buffer, int delay, char* path) {
    var si = 0;
    env.setGate(255);
    while (env.isActive() && si < 20*samplingRate) {
        //LOG("%d - %lf\n", env->phase(), env->timer_);
        if (si == delay) {
            env.setGate(0);
        }
        var smp = env.run();
        buffer[si] = (short)(smp * 32767.0f);
        si++;
    }
    // save to file
    if (path != NULL) {
        WaveFmt::write(path, (int)samplingRate, 1, 16, (byte*)buffer, si << 1);
    }
    return si;
}

SynthTest::SynthTest() {
    samplingRate = 48000.0f;
    saveToFile = false;
    masterDevice_ = PlayerDevice::create(NULL);
    player_ = masterDevice_->player();
    synthAdapter_ = NEW_(SynthAdapter);
    player_->addAdapter(synthAdapter_);
    player_->useThread();
    PlayerDeviceExt::registerExtensionCreator();
    playerExt_ = NEW_(PlayerExt, player_);
    masterDeviceExt_ = (PlayerDeviceExt*)playerExt_->getDeviceExtension(masterDevice_);
}

SynthTest::~SynthTest() {
    synthAdapter_->cleanUp();
    DEL_(masterDevice_);
    DEL_(masterDeviceExt_);
    DEL_(playerExt_);
    Player::cleanUp();
    PlayerExt::cleanUp();
}

void SynthTest::testEnvelopes() {
    Env::initialize();
    // 20 secs buffer
    var size = (int)(20.0f * samplingRate);
    var buffer = MALLOC(short, size);
    //WaveFmt* wave = NULL;
    byte* stream = NULL;
    int sampleCount;

    #pragma region ADSR
    Adsr adsr;
    AdsrValues adsrValues = { 1.0f, 0, 0, 128, 0 };
    adsr.values(&adsrValues);

    sampleCount = testEnvelope(adsr, buffer, 0, "adsr-null.wav");
    assert("Should create null ADSR", sampleCount == 5);   // [48000*0.1ms] = 5

    sampleCount = testEnvelope(adsr, buffer, (int)ceil(2/Adsr::attackRates[0] + 0.1f * samplingRate), "adsr-min.wav");
    assert("Should create minimal ADSR", sampleCount == 4815);   // [48000 * (0.1ms + 0.1ms + 100ms + 0.1ms)] = 4815

    adsrValues.atk = 255;
    sampleCount = testEnvelope(adsr, buffer, (int)ceil(1 / Adsr::attackRates[255] + 1 / Adsr::attackRates[0] + 0.1f * samplingRate), "adsr-atk.wav");
    assert("Should create ADSR with max atk", sampleCount == 196810);   // [48000 * (4s + 0.1ms + 100ms + 0.1ms)] = 196810

    adsrValues.dec = 255;
    sampleCount = testEnvelope(adsr, buffer, (int)ceil(1 / Adsr::attackRates[255] + 1 / Adsr::attackRates[255] + 0.1f * samplingRate), "adsr-atk-dec.wav");
    assert("Should create ADSR with max atk and dec", sampleCount == 388805);   // [48000 * (4s + 4s + 100ms + 0.1ms)] = 388805

    adsrValues.rel = 255;
    sampleCount = testEnvelope(adsr, buffer, (int)ceil(1 / Adsr::attackRates[255] + 1 / Adsr::attackRates[255] + 0.1f * samplingRate), "adsr-atk-dec.wav");
    assert("Should create ADSR with max atk, dec and rel", sampleCount == 772801);   // [48000 * (4s + 4s + 100ms + 8ms)] = 772801

    adsrValues.rel = 0;
    sampleCount = testEnvelope(adsr, buffer, (int)ceil(samplingRate + 0.1f * samplingRate), "adsr-atk-brk.wav");
    assert("Should create ADSR with broken atk", sampleCount == 52805);   // [48000 * (1s + 100ms + 0.1ms)] = 52805

    sampleCount = testEnvelope(adsr, buffer, (int)ceil(samplingRate + 1 / Adsr::attackRates[255] + 0.1f * samplingRate), "adsr-dec-brk.wav");
    assert("Should create ADSR with broken dec", sampleCount == 244805);   // [48000 * (1s + 4s + 100ms + 0.1ms)] = 244805


#pragma endregion

    #pragma region DAHR
    Dahr dahr;
    DahrValues dahrValues = { 1.0f, 0, 0, 0, 0 };
    dahr.values(&dahrValues);

    sampleCount = testEnvelope(dahr, buffer, 0, "dahr-null.wav");
    assert("Should create null DAHR", sampleCount == 6);   // [1 + 48000*0.1ms] = 6

    sampleCount = testEnvelope(dahr, buffer, 48000, "dahr-min.wav");
    assert("Should create minimal DAHR", sampleCount == 20);   // [48000 * (0.1ms + 0.1ms + 0.1ms + 0.1ms)] = 20

    dahrValues.del = 255;
    sampleCount = testEnvelope(dahr, buffer, 960000, "dahr-del.wav");
    assert("Should create DAHR with max del", sampleCount == 192016);   // [48000 * (4s + 0.1s + 0.1ms + 0.1ms)] = 192015

    dahrValues.atk = 255;
    sampleCount = testEnvelope(dahr, buffer, 960000, "dahr-del-atk.wav");
    assert("Should create DAHR with max del and atk", sampleCount == 384012);   // [48000 * (4s + 4ms + 0.1ms + 0.1ms)] = 384010

    dahrValues.hld = 255;
    sampleCount = testEnvelope(dahr, buffer, 960000, "dahr-atk-dec.wav");
    assert("Should create DAHR with max del, atk and hld", sampleCount == 576008);   // [48000 * (4s + 4s + 4s + 0.1ms)] = 576005
    #pragma endregion

    FREE(buffer);
}

void SynthTest::testBass() {
    var bass = (BassDevice*)synthAdapter_->createDevice(SynthDevices::DeviceBass, player_);
    var pb = bass->getDefaultPresetBank();
    bass->setPresetBank(pb);

    playback(bass->module(),
        [](Module* mdl, int frame) {
            int isActive = frame < 32;
            var synth = (SynthBase*)mdl;
            if (isActive) {
                frame %= 8;
                switch (frame) {
                case 0:
                    synth->setNote(pD1, 80);
                    break;
                case 1:
                    synth->setNote(pD1, 0);
                    break;
                case 2:
                    synth->setNote(pD2, 120);
                    break;
                case 3:
                    synth->setNote(pD2, 0);
                    break;
                case 4:
                    synth->setNote(pD2, 160);
                    break;
                case 5:
                    synth->setNote(pD2, 0);
                    break;
                case 6:
                    synth->setNote(pF2, 60);
                    break;
                case 7:
                    synth->setNote(pF2, 0);
                    break;
                }
            }
            return isActive;
        }, 12.0f, "bass1.wav");

    assert("Should create sound", bass->isActive());
    DEL_(bass);
    DEL_(pb);
}

void SynthTest::testBassDeviceExt() {
    BassDeviceExt::registerExtensionCreator();
    var dev = synthAdapter_->createDevice(SynthDevices::DeviceBass, player_);
    var ext = playerExt_->getDeviceExtension(dev);


//    player_->initialize();
//
//    // create master sequence
//    var masterSequence = NEW_(Sequence, masterDevice_);
//    {
//        masterSequence->writeHeader();
//        // #01 frame
//        masterSequence->writeDelta(0);
//        masterSequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(1)->writeByte(16);
//        masterSequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(2)->writeByte(2)->writeByte(1)->writeByte(4);
//        masterSequence->writeEOF();
//        // #01 frame
//        masterSequence->writeDelta(4 * 4 * 8);
//        masterSequence->writeEOS();
//    }
//    player_->addSequence(masterSequence);
//
//    var bassDevice = setupBass();
//
//    player_->refreshRate(16.0f);
//
//    SoundPlayer::start((int)samplingRate, 1, SynthTest::simpleSynthCallback, bassDevice->module());
//    player_->start();
//    while (masterDevice_->isActive()) {
//        Sleep(6);
//    }
//    player_->stop();
//    SoundPlayer::stop();
//
//    assert("Should create sound", bassDevice->isActive());
//    player_->clear();
    DEL_(dev);
    DEL_(ext);
}

//BassDevice* SynthTest::setupBass() {
//    // create device
//    var bassDevice = (BassDevice*)player_->addDevice(synthAdapter_, DeviceBass, NULL);
//    assert("Should create a device", bassDevice != NULL);
//    bassDevice->setProgram(0);
//
//    var bassSequence = NEW_(Sequence, bassDevice);
//    {
//        bassSequence->writeHeader();
//        // #01 frame
//        bassSequence->writeDelta(0)->writeCommand(CmdSetNote)->writeByte(pD0)->writeByte(80)->writeEOF();
//        // #02 frame
//        bassSequence->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pD0)->writeByte(0)->writeEOF();
//        // #03 frame
//        bassSequence->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pD1)->writeByte(120)->writeEOF();
//        // #04 frame
//        bassSequence->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pD1)->writeByte(0)->writeEOF();
//        // #05 frame
//        bassSequence->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pD1)->writeByte(160)->writeEOF();
//        // #06 frame
//        bassSequence->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pD1)->writeByte(0)->writeEOF();
//        // #07 frame
//        bassSequence->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pF1)->writeByte(60)->writeEOF();
//        // #08 frame
//        bassSequence->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pF1)->writeByte(0)->writeEOF();
//        // #09 frame
//        bassSequence->writeDelta(1)->writeEOS();
//    }
//    player_->addSequence(bassSequence);
//
//    var tuneSequence = NEW_(Sequence, bassDevice);
//    {
//        tuneSequence->writeHeader();
//        // #01 frame
//        tuneSequence->writeDelta(0);
//        tuneSequence->writeCommand(CmdSetUint8)->writeByte((int)BassCtrlId::osc1Tune)->writeByte(12);
//        tuneSequence->writeCommand(CmdSetUint8)->writeByte((int)BassCtrlId::osc2Tune)->writeByte(24);
//        tuneSequence->writeEOF();
//        // #02 frame
//        tuneSequence->writeDelta(32);
//        tuneSequence->writeCommand(CmdSetUint8)->writeByte((int)BassCtrlId::osc1Tune)->writeByte(8);
//        tuneSequence->writeCommand(CmdSetUint8)->writeByte((int)BassCtrlId::osc2Tune)->writeByte(20);
//        tuneSequence->writeEOF();
//        // #03 frame
//        tuneSequence->writeDelta(16);
//        tuneSequence->writeCommand(CmdSetUint8)->writeByte((int)BassCtrlId::osc1Tune)->writeByte(10);
//        tuneSequence->writeCommand(CmdSetUint8)->writeByte((int)BassCtrlId::osc2Tune)->writeByte(22);
//        tuneSequence->writeEOF();
//        // #04 frame
//        tuneSequence->writeDelta(16)->writeEOS();
//    }
//    player_->addSequence(tuneSequence);
//
//    var bassChannel = player_->addChannel("bass");
//    var tuneChannel = player_->addChannel("tune");
//
//    return bassDevice;
//}
//
//void SynthTest::testSynth1() {
//    LOG("Test synth1\n");
//    var synth1 = NEW_(Synth, 6);
//    synth1->setProgram(0);
//
//    playback(synth1,
//        [](Module* mdl, int frame) {
//            int isActive = frame < 32;
//            var synth = (Synth*)mdl;
//            if (isActive) {
//                frame %= 8;
//                switch (frame) {
//                case 0:
//                    synth->setNote(pD2, 80);
//                    synth->setNote(pF2, 40);
//                    synth->setNote(pA2, 60);
//                    break;
//                case 1:
//                    synth->setNote(pD2, 0);
//                    synth->setNote(pF2, 0);
//                    synth->setNote(pA2, 0);
//                    break;
//                case 2:
//                    synth->setNote(pD4, 80);
//                    synth->setNote(pF2, 40);
//                    synth->setNote(pA2, 60);
//                    break;
//                case 3:
//                    synth->setNote(pD4, 0);
//                    synth->setNote(pF2, 0);
//                    synth->setNote(pA2, 0);
//                    break;
//                case 4:
//                    synth->setNote(pD2, 80);
//                    synth->setNote(pF2, 40);
//                    synth->setNote(pA2, 60);
//                    break;
//                case 5:
//                    synth->setNote(pD2, 0);
//                    synth->setNote(pF2, 0);
//                    synth->setNote(pA2, 0);
//                    break;
//                case 6:
//                    synth->setNote(pC2, 80);
//                    synth->setNote(pE2, 40);
//                    synth->setNote(pG2, 60);
//                    break;
//                case 7:
//                    synth->setNote(pC2, 0);
//                    synth->setNote(pE2, 0);
//                    synth->setNote(pG2, 0);
//                    break;
//                }
//            }
//            return isActive;
//        }, 4.0f, "synth1.wav");
//
//    assert("Should create sound", synth1->isActive());
//    DEL_(synth1);
//}
//
//void SynthTest::testSynth1Device() {
//    LOG("Test synth1 device\n");
//    // create device
//    player_->initialize();
//    var synth1Device = (SynthDevice*)player_->addDevice(synthAdapter_, DeviceSynth, NULL);
//    synth1Device->synth()->voiceCount(6);
//    assert("Should create a device", synth1Device != NULL);
//    synth1Device->setProgram(0);
//    // create sequences
//    var masterSequence = NEW_(Sequence, masterDevice_);
//    {
//        masterSequence->writeHeader();
//        // #01 frame
//        masterSequence->writeDelta(0);
//        masterSequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(1)->writeByte(4);
//        masterSequence->writeEOF();
//        // #01 frame
//        masterSequence->writeDelta(4 * 8);
//        masterSequence->writeEOS();
//    }
//    player_->addSequence(masterSequence);
//
//    var synth1Sequence = NEW_(Sequence, synth1Device);
//    {
//        synth1Sequence->writeHeader();
//        // #01 frame
//        synth1Sequence->writeDelta(0);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(80);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(40);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pA2)->writeByte(60);
//        synth1Sequence->writeEOF();
//        // #02 frame
//        synth1Sequence->writeDelta(1);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(0);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(0);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pA2)->writeByte(0);
//        synth1Sequence->writeEOF();
//        // #03 frame
//        synth1Sequence->writeDelta(1);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(80);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(40);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pA2)->writeByte(60);
//        synth1Sequence->writeEOF();
//        // #04 frame
//        synth1Sequence->writeDelta(1);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(0);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(0);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pA2)->writeByte(0);
//        synth1Sequence->writeEOF();
//        // #05 frame
//        synth1Sequence->writeDelta(1);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(80);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(40);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pA2)->writeByte(60);
//        synth1Sequence->writeEOF();
//        // #06 frame
//        synth1Sequence->writeDelta(1);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(0);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(0);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pA2)->writeByte(0);
//        synth1Sequence->writeEOF();
//        // #07 frame
//        synth1Sequence->writeDelta(1);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(80);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pE2)->writeByte(40);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pG2)->writeByte(60);
//        synth1Sequence->writeEOF();
//        // #08 frame
//        synth1Sequence->writeDelta(1);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(0);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pE2)->writeByte(0);
//        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pG2)->writeByte(0);
//        synth1Sequence->writeEOF();
//        // #09 frame
//        synth1Sequence->writeDelta(1)->writeEOS();
//    }
//    player_->addSequence(synth1Sequence);
//
//    var synth1Channel = player_->addChannel("synth1");
//    player_->refreshRate(4.0f);
//
//    SoundPlayer::start((int)samplingRate, 1, SynthTest::simpleSynthCallback, synth1Device->module());
//    player_->start();
//    while (masterDevice_->isActive()) {
//        Sleep(6);
//    }
//    player_->stop();
//    SoundPlayer::stop();
//
//    assert("Should create sound", synth1Device->isActive());
//    player_->clear();
//}
//
//void SynthTest::testSynth2() {
//    LOG("Test synth2\n");
//    var synth2 = NEW_(Synth2, 6);
//    synth2->setProgram(0);
//
//    playback(synth2, 
//        [](Module* mdl, int frame) {
//            int isActive = frame < 32;
//            var synth = (Synth*)mdl;
//            if (isActive) {
//                frame %= 8;
//                switch (frame) {
//                case 0:
//                    synth->setNote(pD2, 80);
//                    synth->setNote(pF2, 40);
//                    synth->setNote(pA2, 60);
//                    break;
//                    //case 1:
//                    //    synth->setNote(pD2, 0);
//                    //    synth->setNote(pF2, 0);
//                    //    synth->setNote(pA2, 0);
//                    //    break;
//                case 2:
//                    synth->setNote(pD4, 80);
//                    //synth->setNote(pF2, 40);
//                    //synth->setNote(pA2, 60);
//                    break;
//                case 3:
//                    synth->setNote(pD4, 0);
//                    //synth->setNote(pF2, 0);
//                    //synth->setNote(pA2, 0);
//                    break;
//                    //case 4:
//                    //    synth->setNote(pD2, 80);
//                    //    synth->setNote(pF2, 40);
//                    //    synth->setNote(pA2, 60);
//                    //    break;
//                case 5:
//                    synth->setNote(pD2, 0);
//                    synth->setNote(pF2, 0);
//                    synth->setNote(pA2, 0);
//                    break;
//                case 6:
//                    synth->setNote(pC2, 80);
//                    synth->setNote(pE2, 40);
//                    synth->setNote(pG2, 60);
//                    break;
//                case 7:
//                    synth->setNote(pC2, 0);
//                    synth->setNote(pE2, 0);
//                    synth->setNote(pG2, 0);
//                    break;
//                }
//            }
//            return isActive;
//        }, 4.0f, "synth2.wav");
//
//    assert("Should create sound", synth2->isActive());
//    DEL_(synth2);
//}
//
//void SynthTest::testSynth2Device() {
//    LOG("Test synth2 device\n");
//    player_->initialize();
//    // create device
//    var synth2Device = (SynthDevice*)player_->addDevice(synthAdapter_, DeviceSynth2, NULL);
//    synth2Device->synth()->voiceCount(4);
//    assert("Should create a device", synth2Device != NULL);
//    synth2Device->setProgram(0);
//    // create sequences
//    var masterSequence = NEW_(Sequence, masterDevice_);
//    {
//        masterSequence->writeHeader();
//        // #01 frame
//        masterSequence->writeDelta(0);
//        masterSequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(1)->writeByte(4);
//        masterSequence->writeEOF();
//        // #01 frame
//        masterSequence->writeDelta(4 * 4 * 4);
//        masterSequence->writeEOS();
//    }
//    player_->addSequence(masterSequence);
//
//    var pitch = 1;
//    int b1 = 1, b2 = 3, b3 = 6;
//    var synth2Sequence = NEW_(Sequence, synth2Device);
//    {
//        int ff = 4, f1, f2, f3;
//        synth2Sequence->writeHeader();
//        // #01 frame
//        synth2Sequence->writeDelta(0);
//        // e: 420 2100 2800 - 4 18 23
//        f1 = 5, f2 = 20, f3 = 24;
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt11Cut)->writeByte(ff*f1);
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt1Band)->writeByte(b1);
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt21Cut)->writeByte(ff*f2);
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt2Band)->writeByte(b2);
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt31Cut)->writeByte(ff*f3);
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt3Band)->writeByte(b3);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pD1 + 12*pitch)->writeByte(80);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pF1 + 12*pitch)->writeByte(40);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pA1 + 12*pitch)->writeByte(60);
//        synth2Sequence->writeEOF();
//
//        synth2Sequence->writeDelta(2);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pD1 + 12 * pitch)->writeByte(0);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pF1 + 12 * pitch)->writeByte(0);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pA1 + 12 * pitch)->writeByte(0);
//        synth2Sequence->writeEOF();
//
//        synth2Sequence->writeDelta(2);
//        // a: 800 1400 2500 - 7, 12, 21
//        f1 = 7, f2 = 12, f3 = 21;
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt11Cut)->writeByte(ff * f1);
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt1Band)->writeByte(b1);
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt21Cut)->writeByte(ff * f2);
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt2Band)->writeByte(b2);
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt31Cut)->writeByte(ff * f3);
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt3Band)->writeByte(b3);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pD1 + 12*pitch)->writeByte(80);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pF1 + 12*pitch)->writeByte(40);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pA1 + 12*pitch)->writeByte(60);
//        synth2Sequence->writeEOF();
//
//        synth2Sequence->writeDelta(2);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pD1 + 12*pitch)->writeByte(0);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pF1 + 12*pitch)->writeByte(0);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pA1 + 12*pitch)->writeByte(0);
//        synth2Sequence->writeEOF();
//
//        synth2Sequence->writeDelta(2);
//        // o: 400 950 2300 - 3 8 19
//        f1 = 3, f2 = 8, f3 = 19;
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt11Cut)->writeByte(ff * f1);
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt1Band)->writeByte(b1);
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt21Cut)->writeByte(ff * f2);
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt2Band)->writeByte(b2);
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt31Cut)->writeByte(ff * f3);
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt3Band)->writeByte(b3);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pD1 + 12*pitch)->writeByte(80);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pF1 + 12*pitch)->writeByte(40);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pA1 + 12*pitch)->writeByte(60);
//        synth2Sequence->writeEOF();
//
//        synth2Sequence->writeDelta(2);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pD1 + 12*pitch)->writeByte(0);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pF1 + 12*pitch)->writeByte(0);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pA1 + 12*pitch)->writeByte(0);
//        synth2Sequence->writeEOF();
//
//        synth2Sequence->writeDelta(2);
//        // u: 280 600 2100 - 2, 5, 18
//        f1 = 3, f2 = 4, f3 = 16;
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt11Cut)->writeByte(ff * f1);
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt1Band)->writeByte(b1);
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt21Cut)->writeByte(ff * f2);
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt2Band)->writeByte(b2);
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt31Cut)->writeByte(ff * f3);
//        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt3Band)->writeByte(b3);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pD1 + 12*pitch)->writeByte(80);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pF1 + 12*pitch)->writeByte(40);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pA1 + 12*pitch)->writeByte(60);
//        synth2Sequence->writeEOF();
//
//        synth2Sequence->writeDelta(2);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pD1 + 12*pitch)->writeByte(0);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pF1 + 12*pitch)->writeByte(0);
//        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pA1 + 12*pitch)->writeByte(0);
//        synth2Sequence->writeEOF();
//
//
//        synth2Sequence->writeDelta(2)->writeEOS();
//    }
//    player_->addSequence(synth2Sequence);
//
//    var synth2Channel = player_->addChannel("synth2");
//    player_->refreshRate(8.0f);
//
//    SoundPlayer::start((int)samplingRate, 1, SynthTest::simpleSynthCallback, synth2Device->module());
//    player_->start();
//    while (masterDevice_->isActive()) {
//        Sleep(6);
//    }
//    player_->stop();
//    SoundPlayer::stop();
//
//    assert("Should create sound", synth2Device->isActive());
//    player_->clear();
//}
//
//DrumsDevice* SynthTest::setupDrums() {
//    var drumsDevice = (DrumsDevice*)player_->addDevice(synthAdapter_, DeviceDrums);
//    assert("Should create a device", drumsDevice != NULL);
//    // kick
//    drumsDevice->setProgram(0x0000);
//    // snare
//    drumsDevice->setProgram(0x0101);
//    //drumsDevice->drums()->drums[1].setControl(gdDahr1Amp, 0.0f);
//    // hihats
//    drumsDevice->setProgram(0x0202);
//    //drumsDevice->drums()->drums[2].setControl(gdDahr1Amp, 0.5f);
//    drumsDevice->setProgram(0x0303);
//    //drumsDevice->drums()->drums[3].setControl(gdDahr1Amp, 0.5f);
//    // toms
//    drumsDevice->setProgram(0x0404);
//    drumsDevice->setProgram(0x0505);
//    drumsDevice->setProgram(0x0606);
//    // clap
//    drumsDevice->setProgram(0x0707);
//
//    var drumsSequence = NEW_(Sequence, drumsDevice);
//    {
//        drumsSequence->writeHeader();
//
//        drumsSequence->writeDelta(0);
//        drumsSequence->writeCommand(CmdSetNote)->writeByte(drBD)->writeByte(100);
//        drumsSequence->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(100);
//        drumsSequence->writeEOF();
//
//        drumsSequence->writeDelta(4);
//        drumsSequence->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(160);
//        drumsSequence->writeEOF();
//
//        drumsSequence->writeDelta(4);
//        drumsSequence->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte( 80);
//        drumsSequence->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(160);
//        drumsSequence->writeEOF();
//
//        drumsSequence->writeDelta(4);
//        drumsSequence->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(160);
//        drumsSequence->writeEOF();
//
//        drumsSequence->writeDelta(4);
//        drumsSequence->writeCommand(CmdSetNote)->writeByte(drBD)->writeByte(100);
//        drumsSequence->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(100);
//        drumsSequence->writeEOF();
//
//        drumsSequence->writeDelta(2);
//        drumsSequence->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte(80);
//        drumsSequence->writeCommand(CmdSetNote)->writeByte(drOH)->writeByte(140);
//        drumsSequence->writeEOF();
//
//        drumsSequence->writeDelta(2);
//        drumsSequence->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(160);
//        drumsSequence->writeEOF();
//
//        drumsSequence->writeDelta(4);
//        drumsSequence->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte(80);
//        drumsSequence->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(160);
//        drumsSequence->writeEOF();
//
//        drumsSequence->writeDelta(4);
//        drumsSequence->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(160);
//        drumsSequence->writeEOF();
//
//        drumsSequence->writeDelta(2);
//        drumsSequence->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte(80);
//        drumsSequence->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(160);
//        drumsSequence->writeEOF();
//
//        drumsSequence->writeDelta(2)->writeEOS();
//    }
//    player_->addSequence(drumsSequence);
//
//    var drumsChannel = player_->addChannel("drums");
//
//    return drumsDevice;
//}
//
//void SynthTest::testDrums() {
//    LOG("Test drums\n");
//    var drums = NEW_(Drums);
//    // kick
//    drums->setProgram(0x0000);
//    // snare
//    drums->setProgram(0x0101);
//    //drums->drums[1].setControl(gdDahr1Amp, 0.0f);
//    // hihats
//    drums->setProgram(0x0202);
//    //drums->drums[2].setControl(gdDahr1Amp, 0.5f);
//    drums->setProgram(0x0303);
//    //drums->drums[3].setControl(gdDahr1Amp, 0.5f);
//    // toms
//    drums->setProgram(0x0404);
//    drums->setProgram(0x0505);
//    drums->setProgram(0x0606);
//    // clap
//    drums->setProgram(0x0707);
//
//    playback(drums,
//        [](Module* mdl, int frame) {
//            int isActive = frame < 64;
//            var drums = (Drums*)mdl;
//            if (isActive) {
//                frame %= 16;
//                switch (frame) {
//                case 0:
//                    drums->setNote(drBD, 100);
//                    drums->setNote(drCH, 100);
//                    break;
//                case 2:
//                    drums->setNote(drCH, 160);
//                    break;
//                case 4:
//                    drums->setNote(drSD,  80);
//                    //drums->setNote(drCP, 140);
//                    drums->setNote(drCH, 100);
//                    break;
//                case 6:
//                    drums->setNote(drCH, 160);
//                    break;
//                case 8:
//                    drums->setNote(drBD, 100);
//                    drums->setNote(drCH, 100);
//                    break;
//                case 9:
//                    drums->setNote(drOH, 160);
//                    drums->setNote(drSD, 40);
//                    break;
//                case 10:
//                    drums->setNote(drCH, 160);
//                    break;
//                case 12:
//                    drums->setNote(drSD,  80);
//                    //drums->setNote(drCP, 140);
//                    drums->setNote(drCH, 100);
//                    break;
//                case 14:
//                    drums->setNote(drCH, 160);
//                    break;
//                case 15:
//                    drums->setNote(drSD, 40);
//                    drums->setNote(drCH, 100);
//                    break;
//                }
//            }
//            return isActive;
//        }, 6.0f, "drums.wav");
//
//    assert("Should create sound", drums->isActive());
//    DEL_(drums);
//}
//
//void SynthTest::testDrumsDevice() {
//    LOG("Test drums device\n");
//    player_->initialize();
//
//    var masterSequence = NEW_(Sequence, masterDevice_);
//    {
//        masterSequence->writeHeader();
//        // #01 frame
//        masterSequence->writeDelta(0);
//        masterSequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(1)->writeByte(4);    // drums
//        masterSequence->writeEOF();
//        // #01 frame
//        masterSequence->writeDelta(4 * 16);
//        masterSequence->writeEOS();
//    }
//    player_->addSequence(masterSequence);
//
//    var drumsDevice = setupDrums();
//    var drums = (Drums*)drumsDevice->module();
//
//    player_->refreshRate(16.0f);
//
//    SoundPlayer::start((int)samplingRate, 1, SynthTest::simpleSynthCallback, drums);
//    player_->start();
//    while (masterDevice_->isActive()) {
//        Sleep(6);
//    }
//    player_->stop();
//    SoundPlayer::stop();
//
//    assert("Should create sound", drums->isActive());
//    player_->clear();
//}
//
//void SynthTest::testMixer() {
//    LOG("Test mixer device\n");
//    player_->initialize();
//
//    var masterSequence = NEW_(Sequence, masterDevice_);
//    {
//        masterSequence->writeHeader();
//        // #01 frame
//        masterSequence->writeDelta(0);
//        masterSequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(1)->writeByte(16);    // bass
//        masterSequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(2)->writeByte(2)->writeByte(1)->writeByte(4);    // tune
//        masterSequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(3)->writeByte(3)->writeByte(2)->writeByte(8);    // drums
//        masterSequence->writeEOF();
//        // #01 frame
//        masterSequence->writeDelta(4 * 8 * 4);
//        masterSequence->writeEOS();
//    }
//    player_->addSequence(masterSequence);
//
//
//    var bassDevice = setupBass();
//    var drumsDevice = setupDrums();
//
//    // add mixer
//    var mixerDevice = (MixerDevice*)player_->addDevice(synthAdapter_, DeviceMixer);
//    var mixer = (Mixer8x4*)mixerDevice->module();
//    mixer->channelCount(2);
//    // channel 1
//    mixer->connectInput(mixer->getChannel(0), bassDevice->module());
//    mixer->getControl(MxCh1gain)->value.f = 1.0f;
//    mixer->getControl(MxCh1pan)->value.f = 0.4f;
//    mixer->getControl(MxCh1amp)->value.f = 0.5f;
//
//    // channel 2
//    mixer->connectInput(mixer->getChannel(1), drumsDevice->module());
//    mixer->getControl(MxCh2gain)->value.f = 1.0f;
//    mixer->getControl(MxCh2pan)->value.f = 0.5f;
//    mixer->getControl(MxCh2amp)->value.f = 0.6f;
//
//    player_->refreshRate(16.0f);
//
//    SoundPlayer::start((int)samplingRate, 2, Mixer8x4::fillSoundBuffer, mixer);
//    player_->start();
//    while (masterDevice_->isActive()) {
//        Sleep(6);
//    }
//    player_->stop();
//    SoundPlayer::stop();
//
//    //assert("Should create sound", mixer->isActive());
//    player_->clear();
//}

void SynthTest::runAll(int& totalPassed, int& totalFailed) {
    totalPassed_ = 0;
    totalFailed_ = 0;
    
    test("Test Envelopes", (TestMethod)&SynthTest::testEnvelopes);
    test("Test Bass", (TestMethod)&SynthTest::testBass);
    test("Test Bass export", (TestMethod)&SynthTest::testBassDeviceExt);
    //testSynth1();
    //testSynth1Device();
    //testSynth2();
    //testSynth2Device();
    //testDrums();
    //testDrumsDevice();
    //testMixer();

    totalPassed = totalPassed_;
    totalFailed = totalFailed_;
}