#include "soundlib/src/soundplayer.h"
#include "math.h"
#include "base/memory.h"
#include "base/debug.h"
#include "soundlib/src/wavfmt.h"
#include "player/src/player-lib.h"
#include "player/src/input-ext.h"
#include "synth/test/synth-test.h"

class TestModule : public Module {
public:
    TestModule() {
        RUN = NULL;
        args = NULL;
        values = NULL;
        createOutputBuffers(1);
    };
    void(*RUN)(int start, int end, TestModule* mdl);
    void* args;
    Value* values;
    Value* getValues() { return values; }
    void run(int start, int end, BufferWriteModes mode) { RUN(start, end, this); }
};

class TestModuleDevice : public ModuleDevice {
public:
    TestModuleDevice(Adapter* adapter, Player* player, void* obj) : ModuleDevice(adapter, player, obj) {}
    int getPresetBankSize() { return 0; }
    PresetBank* getDefaultPresetBank() { return NULL; }

};

short* SynthTest::simpleSynthCallback(short* buffer, int sampleCount, void* context) {
    var mdl = (Module*)context;
    mdl->run(0, sampleCount);
    for (var i = 0; i < sampleCount; i++) {
        *buffer++ = (short)(32767.0f * mdl->getOutput(0)[i]);
    }
    return buffer;
}

short* SynthTest::simpleStereoSynthCallback(short* buffer, int sampleCount, void* context) {
    var mdl = (Module*)context;
    mdl->run(0, sampleCount);
    for (var i = 0; i < sampleCount; i++) {
        *buffer++ = (short)(32767.0f * mdl->getOutput(0)[i]);
        *buffer++ = (short)(32767.0f * mdl->getOutput(1)[i]);
    }
    return buffer;
}


void SynthTest::playback(ModuleDevice* dev, PlayCallback callback, float fps, int chn, char* path, FeedSample feedSample, void* context, void* args) {
    var mdl = dev->module();
    int msPerFrame = (int)(1000.0f / fps);
    int frame = 0;
    bool hasError = false;
    double frameSize = samplingRate / fps;
    if (feedSample == NULL) {
        feedSample = SynthTest::simpleSynthCallback;
    }
    if (context == NULL) context = mdl;
    if (args == NULL) args = context;

    if (path != NULL && saveToFile) {
        // prepare wave file and buffer
        var wave = NEW_(WaveFmt, path, (int)samplingRate, chn, 16);
        int sampleCount = (int)frameSize;
        int lengthInBytes = sampleCount * sizeof(short);
        if (chn > 1) lengthInBytes <<= 1;
        var buffer = (short*)MALLOC(byte, lengthInBytes);
        memset(buffer, 0, lengthInBytes);
        //wave->write((byte*)buffer, SAMPLE_BUFFER_SIZE);
        var fileLength = 0;

        // render sound into buffer
        int fi = 0;
        while (callback(dev, mdl, fi++, args)) {
            LOG(".");
            // render 1 frame into the buffer
            //var output = mdl->getOutput(0);
            var p = buffer;
            var len = sampleCount;
            while (len > 0) {
                var toWrite = len > SAMPLE_BUFFER_SIZE ? SAMPLE_BUFFER_SIZE : len;
                p = feedSample((short*)p, toWrite, context);
                //mdl->run(0, toWrite);
                //for (var j = 0; j < toWrite; j++) {
                //    *p++ = (short)(32767.0f * output[j]);
                //}
                len -= toWrite;
            }
            var byteCount = (int)((size_t)p - (size_t)buffer);
            wave->write((byte*)buffer, byteCount);
            fileLength += byteCount;
        }
        wave->close();
        DEL_(wave);
        FREE(buffer);
        LOG("\nSaved %d bytes into '%s'.\n", fileLength, path);
    }
    else {
        if (SoundPlayer::start((int)samplingRate, chn, feedSample, context) == 0) {
            int fi = 0;
            while (callback(dev, mdl, fi++, args)) {
                LOG(".");
                Sleep(msPerFrame);
            }
            Sleep(400);
            SoundPlayer::stop();
            LOG("\nPlayback stopped.\n");
        }
        else {
            LOG("Sound playback error!");
        }
    }
}

bool SynthTest::testInput(InputBase& input, Value min, Value max, Value step, Value* value) {
    var check = false;
    while (true) {
        check = InputExt::compare(&input, 0, min); if (!check) { log("input.min mismatch "); break; }
        check = InputExt::compare(&input, 1, max); if (!check) { log("input.max mismatch "); break; }
        check = InputExt::compare(&input, 2, step); if (!check) { log("input.step mismatch "); break; }
        check = InputExt::compare(&input, 3, *value); if (!check) { log("input.value mismatch "); break; }
        break;
    }
    return check;
}

int SynthTest::testEnvelope(Env& env, short* buffer, void(callback)(int, Env&, float), char* path) {
    int si = 0, offs = (int)(30 * samplingRate);
    var ti = 0;
    while (si < 30*samplingRate) {
        callback(si, env, samplingRate);
        var smp = env.run();
        buffer[si] = (short)(smp * 32767.0f);
        buffer[offs + si] = (short)(smp * sin(ti * 211.3f * SYNTH_THETA / samplingRate) * 32767.0f);
        si++;
        ti++;
        if (!env.isActive()) break;
    }
    // save to file
    if (path != NULL) {
        var path1 = str_concat(path, ".wav");
        var path2 = str_concat("sin-", path1);
        WaveFmt::write(path1, (int)samplingRate, 1, 16, (byte*)buffer, si << 1);
        WaveFmt::write(path2, (int)samplingRate, 1, 16, (byte*)&buffer[offs], si << 1);
        FREE(path1);
        FREE(path2);
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
    player_->initialize();
    player_->addChannel("chn1");
    player_->addChannel("chn2");
    player_->addChannel("chn3");
    player_->addChannel("chn4");
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

void dahrCallback(int d, Env& env, float smpRate) {
    if (d == 0) env.setGate(255);
}

void SynthTest::testEnvelopes() {
    Env::initialize();
    // 2x30 secs buffer
    var size = (int)(2*30.0f * samplingRate);
    var buffer = MALLOC(short, size);
    //WaveFmt* wave = NULL;
    byte* stream = NULL;
    int sampleCount;

    #pragma region ADSR
    Adsr adsr;
    AdsrValues adsrValues = { 1.0f, 0, 0, 0.5f, 0 };
    adsr.setValues(&adsrValues);

    sampleCount = testEnvelope(adsr, buffer, [](int d, Env& env, float smpRate) { }, "adsr-null");
    assert("Should create null ADSR", sampleCount == 1);

    sampleCount = testEnvelope(adsr, buffer, [](int d, Env& env, float smpRate) {
        if (d == 0) env.setGate(255);
        else if (d == (int)(0.0102 * smpRate)) env.setGate(0);
        }, "adsr-min");
    assert("Should create minimal ADSR", sampleCount == 494);   // [0.1ms + 0.1ms + 10ms + 0.1ms] = 10.3ms -> 494,4

    adsrValues.atk = 255;
    sampleCount = testEnvelope(adsr, buffer, [](int d, Env& env, float smpRate) {
        if (d == 0) env.setGate(255);
        else if (d == (int)(4.0101 * smpRate)) env.setGate(0);
        }, "adsr-atk");
    assert("Should create ADSR with max atk", sampleCount == 192489);   // [4s + 0.1ms + 10ms + 0.1ms] = 4010.2ms -> 192489,6

    adsrValues.dec = 255;
    sampleCount = testEnvelope(adsr, buffer, [](int d, Env& env, float smpRate) {
        if (d == 0) env.setGate(255);
        else if (d == (int)(8.01 * smpRate)) env.setGate(0);
        }, "adsr-atk-dec");
    assert("Should create ADSR with max atk and dec", sampleCount == 384485);   // [4s + 4s + 10ms + 0.1ms] = 8010.1 -> 384484,8

    adsrValues.rel = 255;
    sampleCount = testEnvelope(adsr, buffer, [](int d, Env& env, float smpRate) {
        if (d == 0) env.setGate(255);
        else if (d == (int)(8.01 * smpRate)) env.setGate(0);
        }, "adsr-atk-dec-rel");
    assert("Should create ADSR with max atk, dec and rel", sampleCount == 768481);   // [4s + 4s + 10ms + 8s] = 16010ms -> 768480

    sampleCount = testEnvelope(adsr, buffer, [](int d, Env& env, float smpRate) {
        if (d == 0) env.setGate(255);
        else if (d == (int)(2.0 * smpRate)) env.setGate(0);
        }, "adsr-atk-brk");
    assert("Should create ADSR with broken atk", sampleCount == 480001);   // [48000 * (2s + 8s)] = 480001

    sampleCount = testEnvelope(adsr, buffer, [](int d, Env& env, float smpRate) {
        if (d == 0) env.setGate(255);
        else if (d == (int)(6.0 * smpRate)) env.setGate(0);
        }, "adsr-dec-brk");
    assert("Should create ADSR with broken dec", sampleCount == 672001);   // [48000 * (6s + 8s)] = 672001
    #pragma endregion

    #pragma region DAHR
    Dahr dahr;
    DahrValues dahrValues = { 1.0f, 0, 0, 0, 0 };
    dahr.setValues(&dahrValues);

    sampleCount = testEnvelope(dahr, buffer, dahrCallback, "dahr-min");
    assert("Should create minimal DAHR", sampleCount == 21);   // [0.1ms + 0.1ms + 0.1ms + 0.1ms] = 0.4ms -> 19

    dahrValues.del = 255;
    sampleCount = testEnvelope(dahr, buffer, dahrCallback, "dahr-del");
    assert("Should create DAHR with max del", sampleCount == 192017);   // [4s + 0.1ms + 0.1ms + 0.1ms] = 4000.3ms -> 192014,4

    dahrValues.atk = 255;
    sampleCount = testEnvelope(dahr, buffer, dahrCallback, "dahr-del-atk");
    assert("Should create DAHR with max del and atk", sampleCount == 384013);   // [4s + 4s + 0.1ms + 0.1ms] = 8000.2ms -> 384009,6

    dahrValues.hld = 255;
    sampleCount = testEnvelope(dahr, buffer, dahrCallback, "dahr-del-atk-hld");
    assert("Should create DAHR with max del, atk and hld", sampleCount == 576009);   // [4s + 4s + 4s + 0.1ms] = 12000.1ms -> 576004,8

    dahrValues.rel = 255;
    sampleCount = testEnvelope(dahr, buffer, dahrCallback, "dahr-del-atk-hld-rel");
    assert("Should create DAHR with max del, atk and hld", sampleCount == 960005);   // [4s + 4s + 4s + 8ms] = 20000ms -> 960000
    #pragma endregion

    FREE(buffer);
}

void SynthTest::testLfo() {
    Lfo lfo;
    LfoValues lfoValues = { 0.1f, 55.0f };
    lfo.values(&lfoValues);
    TestModule mdl;
    mdl.args = &lfo;
    mdl.RUN = [](int start, int end, TestModule* mdl) {
        var lfo = (Lfo*)mdl->args;
        for (var i = start; i < end; i++) {
            mdl->getOutput(0)[i] = 4.0f * lfo->run();
        }        
    };
    var dev = NEW_(TestModuleDevice, synthAdapter_, player_, &mdl);

    playback(dev,
        [](ModuleDevice* dev, Module* mdl, int frame, void* args) {
            var lfo = (Lfo*)((TestModule*)mdl)->args;
            if (frame % 10 == 0) {
                lfo->values()->fre.f *= 1.189207115f;
            }
            return 40 - frame;
        }, 10.f, 1, "lfo.wav");

    DEL_(dev);
}

void SynthTest::testBass() {
    var bass = (BassDevice*)synthAdapter_->createDevice(SynthDevices::DeviceBass, player_);
    assert("Should assign inputs", (BassInputs*)bass->inputs() == &bass->bassInputs);
    #pragma region check all inputs
    var check = true, checkAll = true;
    log("   Check AM::AMP input "); check = testInput(bass->bassInputs.amAdsr.amp, 0, 1.0f, 0.01f, &bass->module()->getValues()[BassInputAmAmp]); checkAll = checkAll && check; log("\n");
    log("   Check AM::ATK input "); check = testInput(bass->bassInputs.amAdsr.atk, 0, 255, 1, &bass->module()->getValues()[BassInputAmAtk]); checkAll = checkAll && check; log("\n");
    log("   Check AM::DEC input "); check = testInput(bass->bassInputs.amAdsr.dec, 0, 255, 1, &bass->module()->getValues()[BassInputAmDec]); checkAll = checkAll && check; log("\n");
    log("   Check AM::SUS input "); check = testInput(bass->bassInputs.amAdsr.sus, 0, 255, 1, &bass->module()->getValues()[BassInputAmSus]); checkAll = checkAll && check; log("\n");
    log("   Check AM::REL input "); check = testInput(bass->bassInputs.amAdsr.rel, 0, 255, 1, &bass->module()->getValues()[BassInputAmRel]); checkAll = checkAll && check; log("\n");

    log("   Check PM::AMP input "); check = testInput(bass->bassInputs.pmAdsr.amp, 0, 1.0f, 0.01f, &bass->module()->getValues()[BassInputPmAmp]); checkAll = checkAll && check; log("\n");
    log("   Check PM::ATK input "); check = testInput(bass->bassInputs.pmAdsr.atk, 0, 255, 1, &bass->module()->getValues()[BassInputPmAtk]); checkAll = checkAll && check; log("\n");
    log("   Check PM::DEC input "); check = testInput(bass->bassInputs.pmAdsr.dec, 0, 255, 1, &bass->module()->getValues()[BassInputPmDec]); checkAll = checkAll && check; log("\n");
    log("   Check PM::SUS input "); check = testInput(bass->bassInputs.pmAdsr.sus, 0, 255, 1, &bass->module()->getValues()[BassInputPmSus]); checkAll = checkAll && check; log("\n");
    log("   Check PM::REL input "); check = testInput(bass->bassInputs.pmAdsr.rel, 0, 255, 1, &bass->module()->getValues()[BassInputPmRel]); checkAll = checkAll && check; log("\n");

    log("   Check FT::AMP input "); check = testInput(bass->bassInputs.ftAdsr.amp, 0, 1.0f, 0.01f, &bass->module()->getValues()[BassInputFtAmp]); checkAll = checkAll && check; log("\n");
    log("   Check FT::ATK input "); check = testInput(bass->bassInputs.ftAdsr.atk, 0, 255, 1, &bass->module()->getValues()[BassInputFtAtk]); checkAll = checkAll && check; log("\n");
    log("   Check FT::DEC input "); check = testInput(bass->bassInputs.ftAdsr.dec, 0, 255, 1, &bass->module()->getValues()[BassInputFtDec]); checkAll = checkAll && check; log("\n");
    log("   Check FT::SUS input "); check = testInput(bass->bassInputs.ftAdsr.sus, 0, 255, 1, &bass->module()->getValues()[BassInputFtSus]); checkAll = checkAll && check; log("\n");
    log("   Check FT::REL input "); check = testInput(bass->bassInputs.ftAdsr.rel, 0, 255, 1, &bass->module()->getValues()[BassInputFtRel]); checkAll = checkAll && check; log("\n");

    log("   Check OSC1::AMP input "); check = testInput(bass->bassInputs.osc1.amp, 0, 255, 1, &bass->module()->getValues()[BassInputOsc1Amp]); checkAll = checkAll && check; log("\n");
    log("   Check OSC1::FRE input "); check = testInput(bass->bassInputs.osc1.fre, 0, 2000.0f, 0.01f, &bass->module()->getValues()[BassInputOsc1Fre]); checkAll = checkAll && check; log("\n");
    log("   Check OSC1::NOTE input "); check = testInput(bass->bassInputs.osc1.note, 0, 255, 1, &bass->module()->getValues()[BassInputOsc1Note]); checkAll = checkAll && check; log("\n");
    log("   Check OSC1::PSW input "); check = testInput(bass->bassInputs.osc1.psw, 0, 255, 1, &bass->module()->getValues()[BassInputOsc1Psw]); checkAll = checkAll && check; log("\n");
    log("   Check OSC1::TUNE input "); check = testInput(bass->bassInputs.osc1.tune, 0, 255, 1, &bass->module()->getValues()[BassInputOsc1Tune]); checkAll = checkAll && check; log("\n");
    log("   Check OSC1::WAVE input "); check = testInput(bass->bassInputs.osc1.wave, 0, 255, 1, &bass->module()->getValues()[BassInputOsc1Wave]); checkAll = checkAll && check; log("\n");

    log("   Check OSC2::AMP input "); check = testInput(bass->bassInputs.osc2.amp, 0, 255, 1, &bass->module()->getValues()[BassInputOsc2Amp]); checkAll = checkAll && check; log("\n");
    log("   Check OSC2::FRE input "); check = testInput(bass->bassInputs.osc2.fre, 0, 2000.0f, 0.01f, &bass->module()->getValues()[BassInputOsc2Fre]); checkAll = checkAll && check; log("\n");
    log("   Check OSC2::NOTE input "); check = testInput(bass->bassInputs.osc2.note, 0, 255, 1, &bass->module()->getValues()[BassInputOsc2Note]); checkAll = checkAll && check; log("\n");
    log("   Check OSC2::PSW input "); check = testInput(bass->bassInputs.osc2.psw, 0, 255, 1, &bass->module()->getValues()[BassInputOsc2Psw]); checkAll = checkAll && check; log("\n");
    log("   Check OSC2::TUNE input "); check = testInput(bass->bassInputs.osc2.tune, 0, 255, 1, &bass->module()->getValues()[BassInputOsc2Tune]); checkAll = checkAll && check; log("\n");
    log("   Check OSC2::WAVE input "); check = testInput(bass->bassInputs.osc2.wave, 0, 255, 1, &bass->module()->getValues()[BassInputOsc2Wave]); checkAll = checkAll && check; log("\n");

    log("   Check Flt1::CUT input "); check = testInput(bass->bassInputs.flt1.cut, 0, 255, 1, &bass->module()->getValues()[BassInputFlt1Cut]); checkAll = checkAll && check; log("\n");
    log("   Check Flt1::RES input "); check = testInput(bass->bassInputs.flt1.res, 0, 255, 1, &bass->module()->getValues()[BassInputFlt1Res]); checkAll = checkAll && check; log("\n");
    log("   Check Flt1::MODE input "); check = testInput(bass->bassInputs.flt1.mode, 0, 7, 1, &bass->module()->getValues()[BassInputFlt1Mode]); checkAll = checkAll && check; log("\n");

    assert("Should set up inputs correctly", checkAll);
    #pragma endregion

    var pb = bass->getDefaultPresetBank();
    bass->setPresetBank(pb);
    bass->setPreset(0);
    var preset0 = (byte*)pb->values()->get(0);
    cons->dump(preset0, bass->getPresetBankSize(), 16);

    playback(bass,
        [](ModuleDevice* dev, Module* mdl, int frame, void* args) {
            int isActive = frame < 64;
            var synth = (SynthBase*)mdl;
            if (isActive) {
                switch (frame) {
                    case 0:
                        synth->getValues()[BassInputOsc1Tune].b = 2;
                        synth->getValues()[BassInputOsc2Tune].b = 14;
                        break;
                    case 32:
                        synth->getValues()[BassInputOsc1Tune].b = 5;
                        synth->getValues()[BassInputOsc2Tune].b = 17;
                        break;
                    case 48:
                        synth->getValues()[BassInputOsc1Tune].b = 0;
                        synth->getValues()[BassInputOsc2Tune].b = 12;
                        break;

                }
                var step = frame % 8;
                switch (step) {
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
        }, 16.0f, 1, "bass1.wav");

    assert("Should create sound", bass->isActive());
    DEL_(bass);
    DEL_(pb);
}

void SynthTest::playDefaultSequence(SynthBaseDevice* dev, SynthBaseDeviceExt* ext) {
    player_->initialize();
    player_->addChannel("chn1");
    player_->addChannel("chn1a");
    player_->devices().push(dev);
    player_->refreshRate(140.0f);
    var seq = ext->createDefaultSequence();
    var frames = FrameList::fromSequence(seq, ext);
    var time = ((Frame*)frames->get(frames->length() - 1))->time;
    DEL_(frames);

    var ctrlId = dev->type() == SynthDevices::DeviceBass ? BassInputFlt1Cut : SynthInputFlt1Cut;

    var masterSequence = NEW_(Sequence);
    {
        masterSequence->writeHeader();
        // #01 frame
        masterSequence->writeDelta(0);
        masterSequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(1)->writeByte(4);
        masterSequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(2)->writeByte(2)->writeByte(1)->writeByte(4);
        masterSequence->writeEOF();
        // #01 frame
        masterSequence->writeDelta(time * 4);
        masterSequence->writeEOS();
    }
    var modSeq = NEW_(Sequence);
    {
        int step = 4;
        int delta = time * 4 / 256;
        modSeq->writeHeader();
        for (var i = 0; i < 256; i += step) {
            modSeq->writeDelta(delta);
            modSeq->writeCommand(ModuleCommands::CmdSetUint8)->writeByte(ctrlId)->writeByte(i);
            modSeq->writeEOF();

        }
        for (var i = 255; i >= 0; i -= step) {
            modSeq->writeDelta(delta);
            modSeq->writeCommand(ModuleCommands::CmdSetUint8)->writeByte(ctrlId)->writeByte(i);
            modSeq->writeEOF();

        }
        modSeq->writeDelta(delta);
        modSeq->writeEOS();
    }
    player_->addSequence(masterSequence);
    player_->addSequence(seq);
    player_->addSequence(modSeq);

    SoundPlayer::start((int)samplingRate, 1, SynthTest::simpleSynthCallback, dev->module());
    player_->useThread();
    player_->start();
    while (masterDevice_->isActive()) {
        Sleep(10);
    }
    player_->stop();
    SoundPlayer::stop();
    player_->sequences().clear();
    DEL_(masterSequence);
    DEL_(seq);
    DEL_(modSeq);
    player_->devices().pop();
}

void SynthTest::testBassDeviceExt() {
    BassDeviceExt::registerExtensionCreator();
    var bass = (BassDevice*)synthAdapter_->createDevice(SynthDevices::DeviceBass, player_);
    var ext = (BassDeviceExt*)playerExt_->getDeviceExtension(bass);
    var pb = bass->getDefaultPresetBank();
    bass->setPresetBank(pb);

    byte expected[] = { SynthDevices::DeviceBass, 255, 0, 1 };    //type, db-id of preset bank, preset id, voice-count
    Stream received(1024);
    ext->writeToStream(&received);
    var isMatch = binaryCompare(received.data(), received.length(), expected, arraysize(expected));
    assert("Should export device as bytes stream", isMatch);

    received.reset();
    ext->writePresetToStream(&received);
    received.rewind();
    for (var ii = 0; ii < bass->inputCount(); ii++) {
        var input = bass->getInput(ii);
        Value v1, v2;
        switch (input->type) {
            case InputTypeB: v1 = input->value()->b; v2 = received.readByte(); isMatch = v1 == v2;  break;
            case InputTypeF: v1 = input->value()->f; v2 = received.readFloat(); isMatch = fabs(v1.f - v2.f) < FLT_MIN;  break;
            case InputTypeF8: v1 = input->value()->b; v2 = received.readByte(); isMatch = v1 == v2;  break;
        }
        if (!isMatch) {
            log("Input value mismatch at %02x: %04x %04x\n", ii, v1.i, v2.i);
            break;
        }
    }
    
    assert("Should export the preset as byte stream", isMatch);

    #pragma region Check commands
    byte command1[] = { ModuleCommands::CmdSetFloat, BassInputAmAmp, DF(0.5f) };
    byte command2[] = { ModuleCommands::CmdSetUint8, BassInputAmAtk, 0x20 };
    byte command3[] = { ModuleCommands::CmdSetFloat8, BassInputAmSus, 0x40 };
    byte command4[] = { ModuleCommands::CmdSetNote, pA0, 0x80 };
    byte command5[] = { ModuleCommands::CmdSetProgram, 0x00 };

    var cmdStream = ext->makeCommand(ModuleCommands::CmdSetFloat, BassInputAmAmp, 0.5f);
    isMatch = binaryCompare(cmdStream->data(), cmdStream->length(), command1, arraysize(command1));
    assert("Should create the SetFloat command", isMatch);
    DEL_(cmdStream);
    
    cmdStream = ext->makeCommand(ModuleCommands::CmdSetUint8, BassInputAmAtk, 0x20);
    isMatch = binaryCompare(cmdStream->data(), cmdStream->length(), command2, arraysize(command2));
    assert("Should create the SetUint8 command", isMatch);
    DEL_(cmdStream);
    
    cmdStream = ext->makeCommand(ModuleCommands::CmdSetFloat8, BassInputAmSus, 0x40);
    isMatch = binaryCompare(cmdStream->data(), cmdStream->length(), command3, arraysize(command3));
    assert("Should create the SetFloat8 command", isMatch);
    DEL_(cmdStream);
    
    cmdStream = ext->makeCommand(ModuleCommands::CmdSetNote, pA0, 0x80);
    isMatch = binaryCompare(cmdStream->data(), cmdStream->length(), command4, arraysize(command4));
    assert("Should create the SetNote command", isMatch);
    DEL_(cmdStream);

    cmdStream = ext->makeCommand(ModuleCommands::CmdSetProgram, 0x00);
    isMatch = binaryCompare(cmdStream->data(), cmdStream->length(), command5, arraysize(command5));
    assert("Should create the SetProgram command", isMatch);
    DEL_(cmdStream);
    #pragma endregion

    playDefaultSequence(bass, ext);

    DEL_(bass);
    DEL_(ext);
    DEL_(pb);
}

void SynthTest::testSynth1() {
    var synth = (SynthDevice*)synthAdapter_->createDevice(SynthDevices::DeviceSynth, player_);
    assert("Should assign inputs", (SynthInputs*)synth->inputs() == &synth->synthInputs);

    #pragma region check all inputs
    var check = true, checkAll = true;
    log("   Check AM::AMP input "); check = testInput(synth->synthInputs.amAdsr.amp, 0, 1.0f, 0.01f, &synth->module()->getValues()[SynthInputAmEnvAmp]); checkAll = checkAll && check; log("\n");
    log("   Check AM::ATK input "); check = testInput(synth->synthInputs.amAdsr.atk, 0, 255, 1, &synth->module()->getValues()[SynthInputAmEnvAtk]); checkAll = checkAll && check; log("\n");
    log("   Check AM::DEC input "); check = testInput(synth->synthInputs.amAdsr.dec, 0, 255, 1, &synth->module()->getValues()[SynthInputAmEnvDec]); checkAll = checkAll && check; log("\n");
    log("   Check AM::SUS input "); check = testInput(synth->synthInputs.amAdsr.sus, 0, 255, 1, &synth->module()->getValues()[SynthInputAmEnvSus]); checkAll = checkAll && check; log("\n");
    log("   Check AM::REL input "); check = testInput(synth->synthInputs.amAdsr.rel, 0, 255, 1, &synth->module()->getValues()[SynthInputAmEnvRel]); checkAll = checkAll && check; log("\n");

    log("   Check FM::AMP input "); check = testInput(synth->synthInputs.fmAdsr.amp, 0, 1.0f, 0.01f, &synth->module()->getValues()[SynthInputFmEnvAmp]); checkAll = checkAll && check; log("\n");
    log("   Check FM::ATK input "); check = testInput(synth->synthInputs.fmAdsr.atk, 0, 255, 1, &synth->module()->getValues()[SynthInputFmEnvAtk]); checkAll = checkAll && check; log("\n");
    log("   Check FM::DEC input "); check = testInput(synth->synthInputs.fmAdsr.dec, 0, 255, 1, &synth->module()->getValues()[SynthInputFmEnvDec]); checkAll = checkAll && check; log("\n");
    log("   Check FM::SUS input "); check = testInput(synth->synthInputs.fmAdsr.sus, 0, 255, 1, &synth->module()->getValues()[SynthInputFmEnvSus]); checkAll = checkAll && check; log("\n");
    log("   Check FM::REL input "); check = testInput(synth->synthInputs.fmAdsr.rel, 0, 255, 1, &synth->module()->getValues()[SynthInputFmEnvRel]); checkAll = checkAll && check; log("\n");

    log("   Check PM::AMP input "); check = testInput(synth->synthInputs.pmAdsr.amp, 0, 1.0f, 0.01f, &synth->module()->getValues()[SynthInputPmEnvAmp]); checkAll = checkAll && check; log("\n");
    log("   Check PM::ATK input "); check = testInput(synth->synthInputs.pmAdsr.atk, 0, 255, 1, &synth->module()->getValues()[SynthInputPmEnvAtk]); checkAll = checkAll && check; log("\n");
    log("   Check PM::DEC input "); check = testInput(synth->synthInputs.pmAdsr.dec, 0, 255, 1, &synth->module()->getValues()[SynthInputPmEnvDec]); checkAll = checkAll && check; log("\n");
    log("   Check PM::SUS input "); check = testInput(synth->synthInputs.pmAdsr.sus, 0, 255, 1, &synth->module()->getValues()[SynthInputPmEnvSus]); checkAll = checkAll && check; log("\n");
    log("   Check PM::REL input "); check = testInput(synth->synthInputs.pmAdsr.rel, 0, 255, 1, &synth->module()->getValues()[SynthInputPmEnvRel]); checkAll = checkAll && check; log("\n");

    log("   Check FT::AMP input "); check = testInput(synth->synthInputs.ftAdsr.amp, 0, 1.0f, 0.01f, &synth->module()->getValues()[SynthInputFtEnvAmp]); checkAll = checkAll && check; log("\n");
    log("   Check FT::ATK input "); check = testInput(synth->synthInputs.ftAdsr.atk, 0, 255, 1, &synth->module()->getValues()[SynthInputFtEnvAtk]); checkAll = checkAll && check; log("\n");
    log("   Check FT::DEC input "); check = testInput(synth->synthInputs.ftAdsr.dec, 0, 255, 1, &synth->module()->getValues()[SynthInputFtEnvDec]); checkAll = checkAll && check; log("\n");
    log("   Check FT::SUS input "); check = testInput(synth->synthInputs.ftAdsr.sus, 0, 255, 1, &synth->module()->getValues()[SynthInputFtEnvSus]); checkAll = checkAll && check; log("\n");
    log("   Check FT::REL input "); check = testInput(synth->synthInputs.ftAdsr.rel, 0, 255, 1, &synth->module()->getValues()[SynthInputFtEnvRel]); checkAll = checkAll && check; log("\n");

    log("   Check LFO1::AMP input "); check = testInput(synth->synthInputs.lfo1.amp, 0, 100.0f, 0.001f, &synth->module()->getValues()[SynthInputAmLfoAmp]); checkAll = checkAll && check; log("\n");
    log("   Check LFO1::FRE input "); check = testInput(synth->synthInputs.lfo1.fre, 0, 10.0f, 0.01f, &synth->module()->getValues()[SynthInputAmLfoFre]); checkAll = checkAll && check; log("\n");
    log("   Check LFO2::AMP input "); check = testInput(synth->synthInputs.lfo2.amp, 0, 100.0f, 0.001f, &synth->module()->getValues()[SynthInputFmLfoAmp]); checkAll = checkAll && check; log("\n");
    log("   Check LFO2::FRE input "); check = testInput(synth->synthInputs.lfo2.fre, 0, 10.0f, 0.01f, &synth->module()->getValues()[SynthInputFmLfoFre]); checkAll = checkAll && check; log("\n");

    log("   Check OSC1::AMP input "); check = testInput(synth->synthInputs.osc1.amp, 0, 255, 1, &synth->module()->getValues()[SynthInputOsc1Amp]); checkAll = checkAll && check; log("\n");
    log("   Check OSC1::FRE input "); check = testInput(synth->synthInputs.osc1.fre, 0, 2000.0f, 0.01f, &synth->module()->getValues()[SynthInputOsc1Fre]); checkAll = checkAll && check; log("\n");
    log("   Check OSC1::NOTE input "); check = testInput(synth->synthInputs.osc1.note, 0, 255, 1, &synth->module()->getValues()[SynthInputOsc1Note]); checkAll = checkAll && check; log("\n");
    log("   Check OSC1::PSW input "); check = testInput(synth->synthInputs.osc1.psw, 0, 255, 1, &synth->module()->getValues()[SynthInputOsc1Psw]); checkAll = checkAll && check; log("\n");
    log("   Check OSC1::TUNE input "); check = testInput(synth->synthInputs.osc1.tune, 0, 255, 1, &synth->module()->getValues()[SynthInputOsc1Tune]); checkAll = checkAll && check; log("\n");
    log("   Check OSC1::WAVE input "); check = testInput(synth->synthInputs.osc1.wave, 0, 255, 1, &synth->module()->getValues()[SynthInputOsc1Wave]); checkAll = checkAll && check; log("\n");

    log("   Check OSC2::AMP input "); check = testInput(synth->synthInputs.osc2.amp, 0, 255, 1, &synth->module()->getValues()[SynthInputOsc2Amp]); checkAll = checkAll && check; log("\n");
    log("   Check OSC2::FRE input "); check = testInput(synth->synthInputs.osc2.fre, 0, 2000.0f, 0.01f, &synth->module()->getValues()[SynthInputOsc2Fre]); checkAll = checkAll && check; log("\n");
    log("   Check OSC2::NOTE input "); check = testInput(synth->synthInputs.osc2.note, 0, 255, 1, &synth->module()->getValues()[SynthInputOsc2Note]); checkAll = checkAll && check; log("\n");
    log("   Check OSC2::PSW input "); check = testInput(synth->synthInputs.osc2.psw, 0, 255, 1, &synth->module()->getValues()[SynthInputOsc2Psw]); checkAll = checkAll && check; log("\n");
    log("   Check OSC2::TUNE input "); check = testInput(synth->synthInputs.osc2.tune, 0, 255, 1, &synth->module()->getValues()[SynthInputOsc2Tune]); checkAll = checkAll && check; log("\n");
    log("   Check OSC2::WAVE input "); check = testInput(synth->synthInputs.osc2.wave, 0, 255, 1, &synth->module()->getValues()[SynthInputOsc2Wave]); checkAll = checkAll && check; log("\n");

    log("   Check Flt1::CUT input "); check = testInput(synth->synthInputs.flt1.cut, 0, 255, 1, &synth->module()->getValues()[SynthInputFlt1Cut]); checkAll = checkAll && check; log("\n");
    log("   Check Flt1::RES input "); check = testInput(synth->synthInputs.flt1.res, 0, 255, 1, &synth->module()->getValues()[SynthInputFlt1Res]); checkAll = checkAll && check; log("\n");
    log("   Check Flt1::MODE input "); check = testInput(synth->synthInputs.flt1.mode, 0, 7, 1, &synth->module()->getValues()[SynthInputFlt1Mode]); checkAll = checkAll && check; log("\n");

    assert("Should set up inputs correctly", checkAll);
    #pragma endregion

    var pb = synth->getDefaultPresetBank();
    synth->setPresetBank(pb);
    //synth->setPreset(1);

    playback(synth,
        [](ModuleDevice* dev, Module* mdl, int frame, void* args) {
            int isActive = frame < 5;
            var synth = (Synth*)mdl;
            if (isActive) {
                frame %= 8;
                switch (frame) {
                case 0:
                    synth->setNote(pD3, 80);
                    synth->setNote(pF3, 40);
                    synth->setNote(pA3, 60);
                    break;
                case 1:
                    synth->setNote(pD3, 0);
                    synth->setNote(pF3, 0);
                    synth->setNote(pA3, 0);
                    break;
                case 3:
                    synth->setNote(pD3, 80);
                    synth->setNote(pF3, 40);
                    synth->setNote(pA3, 60);
                    break;
                case 4:
                    synth->setNote(pD3, 0);
                    synth->setNote(pF3, 0);
                    synth->setNote(pA3, 0);
                    break;
                case 5:
                    synth->setNote(pC3, 80);
                    synth->setNote(pE3, 40);
                    synth->setNote(pG3, 60);
                    break;
                case 7:
                    synth->setNote(pC3, 0);
                    synth->setNote(pE3, 0);
                    synth->setNote(pG3, 0);
                    break;
                }
            }
            return isActive;
        }, 4.0f, 1, "synth1.wav");

    assert("Should create sound", synth->isActive());
    DEL_(synth);
    DEL_(pb);
}

void SynthTest::testSynth1DeviceExt() {
    SynthDeviceExt::registerExtensionCreator();
    var synth = (SynthDevice*)synthAdapter_->createDevice(SynthDevices::DeviceSynth, player_);
    var ext = (SynthDeviceExt*)playerExt_->getDeviceExtension(synth);
    var pb = synth->getDefaultPresetBank();
    synth->setPresetBank(pb);
    synth->setPreset(1);

    playDefaultSequence(synth, ext);
    
    DEL_(synth);
    DEL_(ext);
    DEL_(pb);

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
}

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
//        }, 4.0f, "synth2");
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

void SynthTest::testGenericDrum() {
    Device* devices[4];
    //GenericDrumValues drumValues[3];
    //memset(&drumValues, 0, 3 * sizeof(GenericDrumValues));
    GenericDrumDevice drum(synthAdapter_, player_);
    var pb = drum.getDefaultPresetBank();
    var mx = (MixerDevice*)synthAdapter_->createDevice(SynthDevices::DeviceMixer, player_);
    devices[3] = mx;
    mx->mixer()->channelCount(3);
    for (var di = 0; di < 3; di++) {
        devices[di] = (GenericDrumDevice*)synthAdapter_->createDevice(SynthDevices::DeviceGenericDrum, player_);
        //((GenericDrumDevice*)devices[di])->drum()->values((Value*)&drumValues[di]);
        //((GenericDrumDevice*)devices[di])->assignInputs();
        //((GenericDrumDevice*)devices[di])->drum()->createOutputBuffers(1);
        devices[di]->setPresetBank(pb);
        devices[di]->setPreset(di);
        mx->mixer()->connectInput(di, (Module*)devices[di]->object(), 0.6f, 128, 200);
    }

    var drdev = (GenericDrumDevice*)devices[0];
    //assert("Should assign inputs", drdev->inputs() == &drdev->drumInputs);

    #pragma region check all inputs
    var check = true, checkAll = true;
    int ix = GenDrumType;
    log("   Check TYPE input "); check = testInput(drdev->drumInputs.type, 0, DrumTypes::ClapType, 1, &drdev->drum()->getValues()[ix++]); checkAll = checkAll && check; log("\n");
    for (var i = 0; i < 4; i++) {
        log("   Check DAHR::AMP input "); check = testInput(drdev->drumInputs.dahr[i].amp, 0, 1.0f, 0.01f, &drdev->drum()->getValues()[ix++]); checkAll = checkAll && check; log("\n");
        log("   Check DAHR::DEL input "); check = testInput(drdev->drumInputs.dahr[i].amp, 0, 1.0f, 0.01f, &drdev->drum()->getValues()[ix++]); checkAll = checkAll && check; log("\n");
        log("   Check DAHR::ATK input "); check = testInput(drdev->drumInputs.dahr[i].amp, 0, 1.0f, 0.01f, &drdev->drum()->getValues()[ix++]); checkAll = checkAll && check; log("\n");
        log("   Check DAHR::HLD input "); check = testInput(drdev->drumInputs.dahr[i].amp, 0, 1.0f, 0.01f, &drdev->drum()->getValues()[ix++]); checkAll = checkAll && check; log("\n");
        log("   Check DAHR::REL input "); check = testInput(drdev->drumInputs.dahr[i].amp, 0, 1.0f, 0.01f, &drdev->drum()->getValues()[ix++]); checkAll = checkAll && check; log("\n");
    }
    for (var i = 0; i < 2; i++) {
        log("   Check Flt::CUT input "); check = testInput(drdev->drumInputs.flt[i].cut, 0, 255, 1, &drdev->drum()->getValues()[ix++]); checkAll = checkAll && check; log("\n");
        log("   Check Flt::RES input "); check = testInput(drdev->drumInputs.flt[i].res, 0, 255, 1, &drdev->drum()->getValues()[ix++]); checkAll = checkAll && check; log("\n");
        log("   Check Flt::MODE input "); check = testInput(drdev->drumInputs.flt[i].mode, 0, 7, 1, &drdev->drum()->getValues()[ix++]); checkAll = checkAll && check; log("\n");
    }

    assert("Should set up inputs correctly", checkAll);
    #pragma endregion


    playback(mx,
        [](ModuleDevice* dev, Module* mdl, int frame, void* args) {
            var dr = (GenericDrumDevice**)args;
            int isActive = frame < 63;
            var step = frame % 16;
            switch (step) {
            case 0:
                dr[0]->drum()->setNote(pC1, 180);
                dr[2]->drum()->setNote(pC3, 100);
                break;
            case 2:
                dr[2]->drum()->setNote(pC3, 60);
                break;
            case 4:
                dr[1]->drum()->setNote(pC2, 170);
                dr[2]->drum()->setNote(pC3, 140);
                break;
            case 6:
                dr[2]->drum()->setNote(pC3, 60);
                break;
            case 8:
                dr[0]->drum()->setNote(pC1, 180);
                dr[2]->drum()->setNote(pC3, 180);
                break;
            case 10:
                dr[0]->drum()->setNote(pC1, 180);
                dr[2]->drum()->setNote(pC3,  60);
                break;
            case 12:
                dr[1]->drum()->setNote(pC2, 170);
                dr[2]->drum()->setNote(pC3, 140);
                break;
            case 14:
                dr[2]->drum()->setNote(pC3, 60);
                break;
            //case 8:
            //    dr[0]->drum()->setNote(pC1, 80);
            //    break;
            //case 10:
            //    dr[0]->drum()->setNote(pC1, 00);
            //    break;
            }
            return isActive;
        }, 8.0f, 2, "drums1.wav",
        [](short* buffer, int count, void* args) {
            var devs = (ModuleDevice**)args;
            //for (var di=0; di<3; di++) devs[di]->module()->run(0, count);
            return Mixer8x4::fillSoundBuffer(buffer, count, devs[3]->module());
        },
        devices);

    for (var di = 0; di < 4; di++) DEL_(devices[di]);
    DEL_(pb);
}

void SynthTest::testDrums() {
    LOG("Test drums\n");
    DrumsDeviceExt::registerExtensionCreator();
    var drums = (DrumsDevice*)synthAdapter_->createDevice(SynthDevices::DeviceDrums, player_);
    var ext = (DrumsDeviceExt*)playerExt_->getDeviceExtension(drums);
    assert("Should assign inputs", (DrumsInputs*)drums->inputs() == &drums->drumsInputs);

    var gdpb = drums->drum()[0]->getDefaultPresetBank();

    drums->drumPresetBanks().push(gdpb);
    var pb = drums->getDefaultPresetBank();
    drums->setPresetBank(pb);
    drums->setPreset(0);
    var preset0 = (byte*)pb->values()->get(0);
    cons->dump(preset0, drums->getPresetBankSize(), 16);

    var seq = ext->createDefaultSequence();
    var chn = NEW_(Channel);
    chn->assign(drums, seq, 4);

    playback(drums,
        [](ModuleDevice* dev, Module* mdl, int frame, void* args) {
            var ch = (Channel*)args;
            ch->run(1);
            //switch (step) {
                //case 0:
                //    dr->setNote(drBD, 0xE0);
                //    dr->setNote(drCH, 0x60);
                //    break;
                //case 2:
                //    dr->setNote(drCH, 0x80);
                //    break;
                //case 4:
                //    dr->setNote(drSD, 0x80);
                //    dr->setNote(drCH, 0x60);
                //    break;
                //case 6:
                //    dr->setNote(drCH, 0x80);
                //    break;
                //case 8:
                //    dr->setNote(drBD, 0xC0);
                //    dr->setNote(drCH, 0x60);
                //    break;
                //case 10:
                //    dr->setNote(drCH, 0x60);
                //    break;
                //case 11:
                //    dr->setNote(drCH, 0x80);
                //    break;
                //case 12:
                //    dr->setNote(drSD, 0x80);
                //    dr->setNote(drCH, 0x60);
                //    break;
                //case 14:
                //    dr->setNote(drCH, 0x80);
                //    break;
                //}
            return (int)ch->isActive();
        }, 80.0f, 2, "drums2.wav",
        SynthTest::simpleStereoSynthCallback,
        NULL, chn);

    assert("Should create sound", drums->isActive());
    DEL_(chn);
    DEL_(seq);
    DEL_(drums);
    DEL_(pb);
    DEL_(ext);
    DEL_(gdpb);
}

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

void SynthTest::testMixer() {
    BassDeviceExt::registerExtensionCreator();
    SynthDeviceExt::registerExtensionCreator();
    DrumsDeviceExt::registerExtensionCreator();
    MixerDeviceExt::registerExtensionCreator();
    player_->initialize();
    player_->useThread();

    #pragma region devices
    // bass
    var bass = (BassDevice*)player_->addDevice(synthAdapter_, SynthDevices::DeviceBass);
    var pb1 = bass->getDefaultPresetBank();
    bass->setPresetBank(pb1);
    bass->setPreset(2);
    // synth
    var synth = (SynthDevice*)player_->addDevice(synthAdapter_, SynthDevices::DeviceSynth);
    synth->synth()->voiceCount(6);
    var pb2 = synth->getDefaultPresetBank();
    synth->setPresetBank(pb2);
    synth->setPreset(0);
    // drums
    var drums = (DrumsDevice*)player_->addDevice(synthAdapter_, SynthDevices::DeviceDrums);
    var gdpb = drums->drum()[0]->getDefaultPresetBank();
    drums->drumPresetBanks().push(gdpb);
    var pb3 = drums->getDefaultPresetBank();
    drums->setPresetBank(pb3);
    // mixer
    var mx = (MixerDevice*)player_->addDevice(synthAdapter_, SynthDevices::DeviceMixer);
    assert("Should assign inputs", (MixerInputs*)mx->inputs() == &mx->mixerInputs);
    // fx1: distortion
    var fx1 = (DistortDevice*)player_->addDevice(synthAdapter_, SynthDevices::DeviceDistort);
    var pb4 = fx1->getDefaultPresetBank();
    fx1->setPresetBank(pb4);
    fx1->setPreset(0);
    // fx2: stereo delay
    var fx2 = (StereoDelayDevice*)player_->addDevice(synthAdapter_, SynthDevices::DeviceStereoDelay);
    var pb5 = fx2->getDefaultPresetBank();
    fx2->setPresetBank(pb5);
    fx2->setPreset(1);
    // fx3: stereo delay
    var fx3 = (StereoDelayDevice*)player_->addDevice(synthAdapter_, SynthDevices::DeviceStereoDelay);
    fx3->setPresetBank(pb5);
    fx3->setPreset(2);
    // fx4: distortion
    var fx4 = (DistortDevice*)player_->addDevice(synthAdapter_, SynthDevices::DeviceDistort);
    fx4->setPresetBank(pb4);
    fx4->setPreset(1);

    mx->mixer()->channelCount(3);
    mx->mixer()->connectInput(0, bass->module(), 0.3f, 110, 10);
    mx->mixer()->connectEffect(0, fx1->module(), 20);
    mx->mixer()->connectEffect(0, fx2->module(), 120);

    mx->mixer()->connectInput(1, synth->module(), 0.4f, 108, 120);
    mx->mixer()->connectEffect(1, fx3->module(), 100);

    mx->mixer()->connectInput(2, drums->module(), 0.8f, 100, 180);
    mx->mixer()->connectEffect(2, fx4->module(), 60);

    player_->addChannel("Chn1");
    player_->addChannel("Chn2");
    player_->addChannel("Chn3");
    player_->addChannel("Chn4");
    #pragma endregion

    #pragma region sequences
    var masterSequence = NEW_(Sequence);
    {
        masterSequence->writeHeader();
        // #01 frame
        masterSequence->writeDelta(0);
        masterSequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(1)->writeByte(16);   // bass
        masterSequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(2)->writeByte(2)->writeByte(2)->writeByte(8);   // tune
        masterSequence->writeEOF();

        masterSequence->writeDelta(2 * 16 * 16);
        masterSequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(3)->writeByte(3)->writeByte(3)->writeByte(12);    // drums
        masterSequence->writeEOF();
        // #01 frame
        masterSequence->writeDelta(7 * 16 * 16);
        masterSequence->writeEOS();
    }
    player_->addSequence(masterSequence);

    for (var di = 1; di < player_->devices().length(); di++) {
        var ext = playerExt_->getDeviceExtension((Device*)player_->devices().get(di));
        if (ext) {
            var seq = ext->createDefaultSequence();
            player_->addSequence(seq);
            DEL_(ext);
        }
    }
    #pragma endregion

    player_->refreshRate(82.0f);
    player_->start();
    SoundPlayer::start((int)samplingRate, 2, Mixer8x4::fillSoundBuffer, mx->mixer());
    while (masterDevice_->isActive()) {
        Sleep(6);
    }
    player_->stop();
    SoundPlayer::stop();
    DEL_(pb1);
    DEL_(pb2);
    DEL_(pb3);
    DEL_(gdpb);
    DEL_(pb4);
    DEL_(pb5);
    player_->clear();

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
}

void SynthTest::testMixerDeviceExt() {

}

void SynthTest::runAll(int& totalPassed, int& totalFailed) {
    totalPassed_ = 0;
    totalFailed_ = 0;
    
    //test("Test Envelopes", (TestMethod)&SynthTest::testEnvelopes);
    //test("Test LFO", (TestMethod)&SynthTest::testLfo);

    //test("Test Bass", (TestMethod)&SynthTest::testBass);
    //test("Test Bass ext", (TestMethod)&SynthTest::testBassDeviceExt);

    //test("Test Synth1", (TestMethod)&SynthTest::testSynth1);
    //test("Test Synth1 ext", (TestMethod)&SynthTest::testSynth1DeviceExt);

    //testSynth2();
    //testSynth2Device();
    
    //test("Test Generic Drum", (TestMethod)&SynthTest::testGenericDrum);

    test("Test Drums", (TestMethod)&SynthTest::testDrums);
    //testDrumsDevice();
    
    test("Test Mixer", (TestMethod)&SynthTest::testMixer);
    //test("Test Mixer ext", (TestMethod)&SynthTest::testMixerDeviceExt);

    totalPassed = totalPassed_;
    totalFailed = totalFailed_;
}