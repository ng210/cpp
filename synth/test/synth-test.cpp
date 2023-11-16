#include <math.h>
#include "synth-test.h"

// remove later
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

int SynthTest::testEnvelope(Env* env, byte* settings, int size, short* buffer, char* path) {
    var si = 0;
    env->setFromStream(settings);
    int noteOff = *(dword*)settings;
    env->setGate(255);
    while (si < size) {
        //LOG("%d - %lf\n", env->phase(), env->timer_);
        if (si == noteOff) {
            env->setGate(0);
        }
        var smp = env->run();
        buffer[si] = (short)(smp * 32767.0f);
        if (!env->isActive()) {
            break;
        }
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
}

SynthTest::~SynthTest() {
    synthAdapter_->cleanUp();
    DEL_(masterDevice_);
    Player::cleanUp();
}

void SynthTest::testEnvelopes() {
    LOG("Test envelopes\n");
    Env::initialize();
    // 20 secs buffer
    var size = (int)(20.0f * samplingRate);
    var buffer = MALLOC(short, size);
    //WaveFmt* wave = NULL;
    byte* stream = NULL;
    int sampleCount;

    #pragma region ADSR
    Adsr adsr;
    AdsrCtrls adsrCtrls;
    adsr.assignControls((PotBase*)&adsrCtrls);
    struct adsrSettings {
        float amp = 1.0f;
        byte atk = 0, dec = 0, sus = 128, rel = 0;
        dword noteoff = 0;
    } adsrSettings;

    adsrSettings.noteoff = (int)ceil(1 / Env::attackRates[0] + 1 / Env::attackRates[0] + 0.100 * samplingRate);
    sampleCount = testEnvelope(&adsr, (byte*)&adsrSettings, size, buffer, "adsr-min.wav");
    assert("Should create minimal ADSR", sampleCount == 4815);   // != 5+5+4800+5 = 4815

    adsrSettings.noteoff = (int)ceil(1 / Env::attackRates[255]);
    adsrSettings.atk = 255;
    sampleCount = testEnvelope(&adsr, (byte*)&adsrSettings, size, buffer, "adsr-atk.wav");
    assert("Should create ADSR with atk", sampleCount == 192005); // != 192000+0+0+5 = 192005

    adsrSettings.noteoff = (int)ceil(1 / Env::attackRates[255] + 1/Env::attackRates[255]);
    adsrSettings.dec = 255;
    sampleCount = testEnvelope(&adsr, (byte*)&adsrSettings, size, buffer, "adsr-dec.wav");
    assert("Should create ADSR with atk,dec", sampleCount == 384005); // != 192000+192000+0+5 = 384005

    adsrSettings.noteoff = (int)ceil(1 / Env::attackRates[255] + 1 / Env::attackRates[255]);
    adsrSettings.rel = 255;
    sampleCount = testEnvelope(&adsr, (byte*)&adsrSettings, size, buffer, "adsr-rel.wav");
    assert("Should create ADSR with atk,dec,rel", sampleCount == 768000); // != 192000+192000+0+384000 = 768000

    adsrSettings.noteoff = (int)ceil(1 / Env::attackRates[255] + 1 / Env::attackRates[255] + 0.1 * samplingRate);
    sampleCount = testEnvelope(&adsr, (byte*)&adsrSettings, size, buffer, "adsr-max.wav");
    assert("Should create ADSR with atk,dec,sus,rel", sampleCount == 772800); // != 192000+192000+4800+384000 = 772800

    adsrSettings.noteoff = (int)ceil(1 / Env::attackRates[128]);
    sampleCount = testEnvelope(&adsr, (byte*)&adsrSettings, size, buffer, "adsr-br-atk.wav");
    assert("Should create ADSR with broken atk", sampleCount == 427093); // != 43093+0+0+384000 = 427093

    adsrSettings.noteoff = (int)ceil(1 / Env::attackRates[255] + 1 / Env::attackRates[128]);
    sampleCount = testEnvelope(&adsr, (byte*)&adsrSettings, size, buffer, "adsr-br-dec.wav");
    assert("Should create ADSR with broken dec", sampleCount == 619093); // != 192000+43093+0+384000 = 619093

#pragma endregion

    #pragma region DAHR
    //Dahr dahr;
    //DahrCtrls dahrCtrls;
    //dahr.assignControls((PotBase*)&dahrCtrls);

    //noteOff = (int)ceil(1 / Env::attackRates[0] + 1 / Env::attackRates[0] + 0.100 * samplingRate);
    //byte dahrMinSettings[] = { DF(1.0f), DB(0), DB(0), DB(0), DB(0), DD(noteOff) };
    //sampleCount = testEnvelope(&dahr, &dahrMinSettings[0], size, buffer);
    //assert("Should create minimal DAHR", sampleCount == 19);   // != 5+5+5+5 = 20

    //noteOff = (int)ceil(1 / Env::attackRates[255] + 1 / Env::attackRates[255] + 0.100 * samplingRate);
    //byte dahrMaxSettings[] = { DF(1.0f), DB(255), DB(255), DB(128), DB(255), DD(noteOff) };
    //sampleCount = testEnvelope(&dahr, &dahrMaxSettings[0], size, buffer);
    //assert("Should create maximal DAHR", sampleCount == 772801); // != 192000+192000+4800+384000 = 772800

    //noteOff = (int)ceil(1 / Env::attackRates[128] + 1 / Env::attackRates[128] + 0.100 * samplingRate);
    //byte dahrMidSettings[] = { DF(1.0f), DB(128), DB(128), DB(128), DB(128), DD(noteOff) };
    //sampleCount = testEnvelope(&dahr, &dahrMidSettings[0], size, buffer);
    //assert("Should create medium DAHR", sampleCount == 177168); // != 43093+43093+4800+86181 = 177168

    //noteOff = (int)ceil(1 / Env::attackRates[128] + 1 / Env::attackRates[128] + 0.100 * samplingRate);
    //byte dahrSus80Settings[] = { DF(1.0f), DB(128), DB(128), DB(204), DB(128), DD(noteOff) };
    //sampleCount = testEnvelope(&dahr, &dahrSus80Settings[0], size, buffer);
    //assert("Should create medium DAHR with 0.8 sustain", sampleCount == 177168); // != 43093+43093+4800+86181 = 177168
    #pragma endregion

    FREE(buffer);
}

void SynthTest::testBass() {
    LOG("Test bass\n");
    var bass = NEW_(Bass, 1);
    bass->setProgram(0);

    playback(bass,
        [](Module* mdl, int frame) {
            int isActive = frame < 32;
            var synth = (Synth*)mdl;
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
}

BassDevice* SynthTest::setupBass() {
    // create device
    var bassDevice = (BassDevice*)player_->addDevice(synthAdapter_, DeviceBass, NULL);
    assert("Should create a device", bassDevice != NULL);
    bassDevice->setProgram(0);

    var bassSequence = NEW_(Sequence, bassDevice);
    {
        bassSequence->writeHeader();
        // #01 frame
        bassSequence->writeDelta(0)->writeCommand(CmdSetNote)->writeByte(pD0)->writeByte(80)->writeEOF();
        // #02 frame
        bassSequence->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pD0)->writeByte(0)->writeEOF();
        // #03 frame
        bassSequence->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pD1)->writeByte(120)->writeEOF();
        // #04 frame
        bassSequence->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pD1)->writeByte(0)->writeEOF();
        // #05 frame
        bassSequence->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pD1)->writeByte(160)->writeEOF();
        // #06 frame
        bassSequence->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pD1)->writeByte(0)->writeEOF();
        // #07 frame
        bassSequence->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pF1)->writeByte(60)->writeEOF();
        // #08 frame
        bassSequence->writeDelta(1)->writeCommand(CmdSetNote)->writeByte(pF1)->writeByte(0)->writeEOF();
        // #09 frame
        bassSequence->writeDelta(1)->writeEOS();
    }
    player_->addSequence(bassSequence);

    var tuneSequence = NEW_(Sequence, bassDevice);
    {
        tuneSequence->writeHeader();
        // #01 frame
        tuneSequence->writeDelta(0);
        tuneSequence->writeCommand(CmdSetUint8)->writeByte((int)BassCtrlId::osc1Tune)->writeByte(12);
        tuneSequence->writeCommand(CmdSetUint8)->writeByte((int)BassCtrlId::osc2Tune)->writeByte(24);
        tuneSequence->writeEOF();
        // #02 frame
        tuneSequence->writeDelta(32);
        tuneSequence->writeCommand(CmdSetUint8)->writeByte((int)BassCtrlId::osc1Tune)->writeByte(8);
        tuneSequence->writeCommand(CmdSetUint8)->writeByte((int)BassCtrlId::osc2Tune)->writeByte(20);
        tuneSequence->writeEOF();
        // #03 frame
        tuneSequence->writeDelta(16);
        tuneSequence->writeCommand(CmdSetUint8)->writeByte((int)BassCtrlId::osc1Tune)->writeByte(10);
        tuneSequence->writeCommand(CmdSetUint8)->writeByte((int)BassCtrlId::osc2Tune)->writeByte(22);
        tuneSequence->writeEOF();
        // #04 frame
        tuneSequence->writeDelta(16)->writeEOS();
    }
    player_->addSequence(tuneSequence);

    var bassChannel = player_->addChannel("bass");
    var tuneChannel = player_->addChannel("tune");

    return bassDevice;
}

void SynthTest::testBassDevice() {
    LOG("Test bass device\n");
    player_->initialize();

    // create master sequence
    var masterSequence = NEW_(Sequence, masterDevice_);
    {
        masterSequence->writeHeader();
        // #01 frame
        masterSequence->writeDelta(0);
        masterSequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(1)->writeByte(16);
        masterSequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(2)->writeByte(2)->writeByte(1)->writeByte(4);
        masterSequence->writeEOF();
        // #01 frame
        masterSequence->writeDelta(4 * 4 * 8);
        masterSequence->writeEOS();
    }
    player_->addSequence(masterSequence);

    var bassDevice = setupBass();

    player_->refreshRate(16.0f);

    SoundPlayer::start((int)samplingRate, 1, SynthTest::simpleSynthCallback, bassDevice->module());
    player_->start();
    while (masterDevice_->isActive()) {
        Sleep(6);
    }
    player_->stop();
    SoundPlayer::stop();

    assert("Should create sound", bassDevice->isActive());
    player_->clear();
}

void SynthTest::testSynth1() {
    LOG("Test synth1\n");
    var synth1 = NEW_(Synth, 6);
    synth1->setProgram(0);

    playback(synth1,
        [](Module* mdl, int frame) {
            int isActive = frame < 32;
            var synth = (Synth*)mdl;
            if (isActive) {
                frame %= 8;
                switch (frame) {
                case 0:
                    synth->setNote(pD2, 80);
                    synth->setNote(pF2, 40);
                    synth->setNote(pA2, 60);
                    break;
                case 1:
                    synth->setNote(pD2, 0);
                    synth->setNote(pF2, 0);
                    synth->setNote(pA2, 0);
                    break;
                case 2:
                    synth->setNote(pD4, 80);
                    synth->setNote(pF2, 40);
                    synth->setNote(pA2, 60);
                    break;
                case 3:
                    synth->setNote(pD4, 0);
                    synth->setNote(pF2, 0);
                    synth->setNote(pA2, 0);
                    break;
                case 4:
                    synth->setNote(pD2, 80);
                    synth->setNote(pF2, 40);
                    synth->setNote(pA2, 60);
                    break;
                case 5:
                    synth->setNote(pD2, 0);
                    synth->setNote(pF2, 0);
                    synth->setNote(pA2, 0);
                    break;
                case 6:
                    synth->setNote(pC2, 80);
                    synth->setNote(pE2, 40);
                    synth->setNote(pG2, 60);
                    break;
                case 7:
                    synth->setNote(pC2, 0);
                    synth->setNote(pE2, 0);
                    synth->setNote(pG2, 0);
                    break;
                }
            }
            return isActive;
        }, 4.0f, "synth1.wav");

    assert("Should create sound", synth1->isActive());
    DEL_(synth1);
}

void SynthTest::testSynth1Device() {
    LOG("Test synth1 device\n");
    // create device
    player_->initialize();
    var synth1Device = (SynthDevice*)player_->addDevice(synthAdapter_, DeviceSynth, NULL);
    synth1Device->synth()->voiceCount(6);
    assert("Should create a device", synth1Device != NULL);
    synth1Device->setProgram(0);
    // create sequences
    var masterSequence = NEW_(Sequence, masterDevice_);
    {
        masterSequence->writeHeader();
        // #01 frame
        masterSequence->writeDelta(0);
        masterSequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(1)->writeByte(4);
        masterSequence->writeEOF();
        // #01 frame
        masterSequence->writeDelta(4 * 8);
        masterSequence->writeEOS();
    }
    player_->addSequence(masterSequence);

    var synth1Sequence = NEW_(Sequence, synth1Device);
    {
        synth1Sequence->writeHeader();
        // #01 frame
        synth1Sequence->writeDelta(0);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(80);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(40);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pA2)->writeByte(60);
        synth1Sequence->writeEOF();
        // #02 frame
        synth1Sequence->writeDelta(1);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(0);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(0);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pA2)->writeByte(0);
        synth1Sequence->writeEOF();
        // #03 frame
        synth1Sequence->writeDelta(1);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(80);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(40);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pA2)->writeByte(60);
        synth1Sequence->writeEOF();
        // #04 frame
        synth1Sequence->writeDelta(1);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(0);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(0);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pA2)->writeByte(0);
        synth1Sequence->writeEOF();
        // #05 frame
        synth1Sequence->writeDelta(1);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(80);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(40);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pA2)->writeByte(60);
        synth1Sequence->writeEOF();
        // #06 frame
        synth1Sequence->writeDelta(1);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(0);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(0);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pA2)->writeByte(0);
        synth1Sequence->writeEOF();
        // #07 frame
        synth1Sequence->writeDelta(1);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(80);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pE2)->writeByte(40);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pG2)->writeByte(60);
        synth1Sequence->writeEOF();
        // #08 frame
        synth1Sequence->writeDelta(1);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(0);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pE2)->writeByte(0);
        synth1Sequence->writeCommand(CmdSetNote)->writeByte(pG2)->writeByte(0);
        synth1Sequence->writeEOF();
        // #09 frame
        synth1Sequence->writeDelta(1)->writeEOS();
    }
    player_->addSequence(synth1Sequence);

    var synth1Channel = player_->addChannel("synth1");
    player_->refreshRate(4.0f);

    SoundPlayer::start((int)samplingRate, 1, SynthTest::simpleSynthCallback, synth1Device->module());
    player_->start();
    while (masterDevice_->isActive()) {
        Sleep(6);
    }
    player_->stop();
    SoundPlayer::stop();

    assert("Should create sound", synth1Device->isActive());
    player_->clear();
}

void SynthTest::testSynth2() {
    LOG("Test synth2\n");
    var synth2 = NEW_(Synth2, 6);
    synth2->setProgram(0);

    playback(synth2, 
        [](Module* mdl, int frame) {
            int isActive = frame < 32;
            var synth = (Synth*)mdl;
            if (isActive) {
                frame %= 8;
                switch (frame) {
                case 0:
                    synth->setNote(pD2, 80);
                    synth->setNote(pF2, 40);
                    synth->setNote(pA2, 60);
                    break;
                    //case 1:
                    //    synth->setNote(pD2, 0);
                    //    synth->setNote(pF2, 0);
                    //    synth->setNote(pA2, 0);
                    //    break;
                case 2:
                    synth->setNote(pD4, 80);
                    //synth->setNote(pF2, 40);
                    //synth->setNote(pA2, 60);
                    break;
                case 3:
                    synth->setNote(pD4, 0);
                    //synth->setNote(pF2, 0);
                    //synth->setNote(pA2, 0);
                    break;
                    //case 4:
                    //    synth->setNote(pD2, 80);
                    //    synth->setNote(pF2, 40);
                    //    synth->setNote(pA2, 60);
                    //    break;
                case 5:
                    synth->setNote(pD2, 0);
                    synth->setNote(pF2, 0);
                    synth->setNote(pA2, 0);
                    break;
                case 6:
                    synth->setNote(pC2, 80);
                    synth->setNote(pE2, 40);
                    synth->setNote(pG2, 60);
                    break;
                case 7:
                    synth->setNote(pC2, 0);
                    synth->setNote(pE2, 0);
                    synth->setNote(pG2, 0);
                    break;
                }
            }
            return isActive;
        }, 4.0f, "synth2.wav");

    assert("Should create sound", synth2->isActive());
    DEL_(synth2);
}

void SynthTest::testSynth2Device() {
    LOG("Test synth2 device\n");
    player_->initialize();
    // create device
    var synth2Device = (SynthDevice*)player_->addDevice(synthAdapter_, DeviceSynth2, NULL);
    synth2Device->synth()->voiceCount(4);
    assert("Should create a device", synth2Device != NULL);
    synth2Device->setProgram(0);
    // create sequences
    var masterSequence = NEW_(Sequence, masterDevice_);
    {
        masterSequence->writeHeader();
        // #01 frame
        masterSequence->writeDelta(0);
        masterSequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(1)->writeByte(4);
        masterSequence->writeEOF();
        // #01 frame
        masterSequence->writeDelta(4 * 4 * 4);
        masterSequence->writeEOS();
    }
    player_->addSequence(masterSequence);

    var pitch = 1;
    int b1 = 1, b2 = 3, b3 = 6;
    var synth2Sequence = NEW_(Sequence, synth2Device);
    {
        int ff = 4, f1, f2, f3;
        synth2Sequence->writeHeader();
        // #01 frame
        synth2Sequence->writeDelta(0);
        // e: 420 2100 2800 - 4 18 23
        f1 = 5, f2 = 20, f3 = 24;
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt11Cut)->writeByte(ff*f1);
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt1Band)->writeByte(b1);
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt21Cut)->writeByte(ff*f2);
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt2Band)->writeByte(b2);
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt31Cut)->writeByte(ff*f3);
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt3Band)->writeByte(b3);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pD1 + 12*pitch)->writeByte(80);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pF1 + 12*pitch)->writeByte(40);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pA1 + 12*pitch)->writeByte(60);
        synth2Sequence->writeEOF();

        synth2Sequence->writeDelta(2);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pD1 + 12 * pitch)->writeByte(0);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pF1 + 12 * pitch)->writeByte(0);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pA1 + 12 * pitch)->writeByte(0);
        synth2Sequence->writeEOF();

        synth2Sequence->writeDelta(2);
        // a: 800 1400 2500 - 7, 12, 21
        f1 = 7, f2 = 12, f3 = 21;
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt11Cut)->writeByte(ff * f1);
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt1Band)->writeByte(b1);
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt21Cut)->writeByte(ff * f2);
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt2Band)->writeByte(b2);
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt31Cut)->writeByte(ff * f3);
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt3Band)->writeByte(b3);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pD1 + 12*pitch)->writeByte(80);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pF1 + 12*pitch)->writeByte(40);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pA1 + 12*pitch)->writeByte(60);
        synth2Sequence->writeEOF();

        synth2Sequence->writeDelta(2);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pD1 + 12*pitch)->writeByte(0);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pF1 + 12*pitch)->writeByte(0);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pA1 + 12*pitch)->writeByte(0);
        synth2Sequence->writeEOF();

        synth2Sequence->writeDelta(2);
        // o: 400 950 2300 - 3 8 19
        f1 = 3, f2 = 8, f3 = 19;
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt11Cut)->writeByte(ff * f1);
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt1Band)->writeByte(b1);
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt21Cut)->writeByte(ff * f2);
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt2Band)->writeByte(b2);
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt31Cut)->writeByte(ff * f3);
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt3Band)->writeByte(b3);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pD1 + 12*pitch)->writeByte(80);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pF1 + 12*pitch)->writeByte(40);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pA1 + 12*pitch)->writeByte(60);
        synth2Sequence->writeEOF();

        synth2Sequence->writeDelta(2);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pD1 + 12*pitch)->writeByte(0);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pF1 + 12*pitch)->writeByte(0);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pA1 + 12*pitch)->writeByte(0);
        synth2Sequence->writeEOF();

        synth2Sequence->writeDelta(2);
        // u: 280 600 2100 - 2, 5, 18
        f1 = 3, f2 = 4, f3 = 16;
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt11Cut)->writeByte(ff * f1);
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt1Band)->writeByte(b1);
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt21Cut)->writeByte(ff * f2);
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt2Band)->writeByte(b2);
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt31Cut)->writeByte(ff * f3);
        synth2Sequence->writeCommand(CmdSetUint8)->writeByte((int)Synth2CtrlId::flt3Band)->writeByte(b3);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pD1 + 12*pitch)->writeByte(80);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pF1 + 12*pitch)->writeByte(40);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pA1 + 12*pitch)->writeByte(60);
        synth2Sequence->writeEOF();

        synth2Sequence->writeDelta(2);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pD1 + 12*pitch)->writeByte(0);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pF1 + 12*pitch)->writeByte(0);
        synth2Sequence->writeCommand(CmdSetNote)->writeByte(pA1 + 12*pitch)->writeByte(0);
        synth2Sequence->writeEOF();


        synth2Sequence->writeDelta(2)->writeEOS();
    }
    player_->addSequence(synth2Sequence);

    var synth2Channel = player_->addChannel("synth2");
    player_->refreshRate(8.0f);

    SoundPlayer::start((int)samplingRate, 1, SynthTest::simpleSynthCallback, synth2Device->module());
    player_->start();
    while (masterDevice_->isActive()) {
        Sleep(6);
    }
    player_->stop();
    SoundPlayer::stop();

    assert("Should create sound", synth2Device->isActive());
    player_->clear();
}

DrumsDevice* SynthTest::setupDrums() {
    var drumsDevice = (DrumsDevice*)player_->addDevice(synthAdapter_, DeviceDrums);
    assert("Should create a device", drumsDevice != NULL);
    // kick
    drumsDevice->setProgram(0x0000);
    // snare
    drumsDevice->setProgram(0x0101);
    //drumsDevice->drums()->drums[1].setControl(gdDahr1Amp, 0.0f);
    // hihats
    drumsDevice->setProgram(0x0202);
    //drumsDevice->drums()->drums[2].setControl(gdDahr1Amp, 0.5f);
    drumsDevice->setProgram(0x0303);
    //drumsDevice->drums()->drums[3].setControl(gdDahr1Amp, 0.5f);
    // toms
    drumsDevice->setProgram(0x0404);
    drumsDevice->setProgram(0x0505);
    drumsDevice->setProgram(0x0606);
    // clap
    drumsDevice->setProgram(0x0707);

    var drumsSequence = NEW_(Sequence, drumsDevice);
    {
        drumsSequence->writeHeader();

        drumsSequence->writeDelta(0);
        drumsSequence->writeCommand(CmdSetNote)->writeByte(drBD)->writeByte(100);
        drumsSequence->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(100);
        drumsSequence->writeEOF();

        drumsSequence->writeDelta(4);
        drumsSequence->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(160);
        drumsSequence->writeEOF();

        drumsSequence->writeDelta(4);
        drumsSequence->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte( 80);
        drumsSequence->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(160);
        drumsSequence->writeEOF();

        drumsSequence->writeDelta(4);
        drumsSequence->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(160);
        drumsSequence->writeEOF();

        drumsSequence->writeDelta(4);
        drumsSequence->writeCommand(CmdSetNote)->writeByte(drBD)->writeByte(100);
        drumsSequence->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(100);
        drumsSequence->writeEOF();

        drumsSequence->writeDelta(2);
        drumsSequence->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte(80);
        drumsSequence->writeCommand(CmdSetNote)->writeByte(drOH)->writeByte(140);
        drumsSequence->writeEOF();

        drumsSequence->writeDelta(2);
        drumsSequence->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(160);
        drumsSequence->writeEOF();

        drumsSequence->writeDelta(4);
        drumsSequence->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte(80);
        drumsSequence->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(160);
        drumsSequence->writeEOF();

        drumsSequence->writeDelta(4);
        drumsSequence->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(160);
        drumsSequence->writeEOF();

        drumsSequence->writeDelta(2);
        drumsSequence->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte(80);
        drumsSequence->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(160);
        drumsSequence->writeEOF();

        drumsSequence->writeDelta(2)->writeEOS();
    }
    player_->addSequence(drumsSequence);

    var drumsChannel = player_->addChannel("drums");

    return drumsDevice;
}

void SynthTest::testDrums() {
    LOG("Test drums\n");
    var drums = NEW_(Drums);
    // kick
    drums->setProgram(0x0000);
    // snare
    drums->setProgram(0x0101);
    //drums->drums[1].setControl(gdDahr1Amp, 0.0f);
    // hihats
    drums->setProgram(0x0202);
    //drums->drums[2].setControl(gdDahr1Amp, 0.5f);
    drums->setProgram(0x0303);
    //drums->drums[3].setControl(gdDahr1Amp, 0.5f);
    // toms
    drums->setProgram(0x0404);
    drums->setProgram(0x0505);
    drums->setProgram(0x0606);
    // clap
    drums->setProgram(0x0707);

    playback(drums,
        [](Module* mdl, int frame) {
            int isActive = frame < 64;
            var drums = (Drums*)mdl;
            if (isActive) {
                frame %= 16;
                switch (frame) {
                case 0:
                    drums->setNote(drBD, 100);
                    drums->setNote(drCH, 100);
                    break;
                case 2:
                    drums->setNote(drCH, 160);
                    break;
                case 4:
                    drums->setNote(drSD,  80);
                    //drums->setNote(drCP, 140);
                    drums->setNote(drCH, 100);
                    break;
                case 6:
                    drums->setNote(drCH, 160);
                    break;
                case 8:
                    drums->setNote(drBD, 100);
                    drums->setNote(drCH, 100);
                    break;
                case 9:
                    drums->setNote(drOH, 160);
                    drums->setNote(drSD, 40);
                    break;
                case 10:
                    drums->setNote(drCH, 160);
                    break;
                case 12:
                    drums->setNote(drSD,  80);
                    //drums->setNote(drCP, 140);
                    drums->setNote(drCH, 100);
                    break;
                case 14:
                    drums->setNote(drCH, 160);
                    break;
                case 15:
                    drums->setNote(drSD, 40);
                    drums->setNote(drCH, 100);
                    break;
                }
            }
            return isActive;
        }, 6.0f, "drums.wav");

    assert("Should create sound", drums->isActive());
    DEL_(drums);
}

void SynthTest::testDrumsDevice() {
    LOG("Test drums device\n");
    player_->initialize();

    var masterSequence = NEW_(Sequence, masterDevice_);
    {
        masterSequence->writeHeader();
        // #01 frame
        masterSequence->writeDelta(0);
        masterSequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(1)->writeByte(4);    // drums
        masterSequence->writeEOF();
        // #01 frame
        masterSequence->writeDelta(4 * 16);
        masterSequence->writeEOS();
    }
    player_->addSequence(masterSequence);

    var drumsDevice = setupDrums();
    var drums = (Drums*)drumsDevice->module();

    player_->refreshRate(16.0f);

    SoundPlayer::start((int)samplingRate, 1, SynthTest::simpleSynthCallback, drums);
    player_->start();
    while (masterDevice_->isActive()) {
        Sleep(6);
    }
    player_->stop();
    SoundPlayer::stop();

    assert("Should create sound", drums->isActive());
    player_->clear();
}

void SynthTest::testMixer() {
    LOG("Test mixer device\n");
    player_->initialize();

    var masterSequence = NEW_(Sequence, masterDevice_);
    {
        masterSequence->writeHeader();
        // #01 frame
        masterSequence->writeDelta(0);
        masterSequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(1)->writeByte(16);    // bass
        masterSequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(2)->writeByte(2)->writeByte(1)->writeByte(4);    // tune
        masterSequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(3)->writeByte(3)->writeByte(2)->writeByte(8);    // drums
        masterSequence->writeEOF();
        // #01 frame
        masterSequence->writeDelta(4 * 8 * 4);
        masterSequence->writeEOS();
    }
    player_->addSequence(masterSequence);


    var bassDevice = setupBass();
    var drumsDevice = setupDrums();

    // add mixer
    var mixerDevice = (MixerDevice*)player_->addDevice(synthAdapter_, DeviceMixer);
    var mixer = (Mixer8x4*)mixerDevice->module();
    mixer->channelCount(2);
    // channel 1
    mixer->connectInput(mixer->getChannel(0), bassDevice->module());
    mixer->getControl(MxCh1gain)->value.f = 1.0f;
    mixer->getControl(MxCh1pan)->value.f = 0.4f;
    mixer->getControl(MxCh1amp)->value.f = 0.5f;

    // channel 2
    mixer->connectInput(mixer->getChannel(1), drumsDevice->module());
    mixer->getControl(MxCh2gain)->value.f = 1.0f;
    mixer->getControl(MxCh2pan)->value.f = 0.5f;
    mixer->getControl(MxCh2amp)->value.f = 0.6f;

    player_->refreshRate(16.0f);

    SoundPlayer::start((int)samplingRate, 2, Mixer8x4::fillSoundBuffer, mixer);
    player_->start();
    while (masterDevice_->isActive()) {
        Sleep(6);
    }
    player_->stop();
    SoundPlayer::stop();

    //assert("Should create sound", mixer->isActive());
    player_->clear();
}

void SynthTest::runAll(int& totalPassed, int& totalFailed) {
    totalPassed_ = 0;
    totalFailed_ = 0;
    
    //testEnvelopes();
    testBass();
    testBassDevice();
    testSynth1();
    testSynth1Device();
    testSynth2();
    testSynth2Device();
    testDrums();
    testDrumsDevice();
    testMixer();

    totalPassed = totalPassed_;
    totalFailed = totalFailed_;
}