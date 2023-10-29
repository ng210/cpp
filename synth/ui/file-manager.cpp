#include "synth/ui/file-manager.h"
#include "synth/ui/synth-app.h"
#include "base/stream.h"
#include "synth/src/device/module-device.h"
#include "synth/src/xmloader.h"

NS_FW_BASE_USE
using namespace SYNTH;
using namespace SYNTH_APP;

FileManager::FileManager(SynthApp* app) : Component(app) {

}

FileManager::~FileManager() {

}


#pragma region Binary
PlayerDevice* FileManager::loadDefault() {
    var playerDevice = PlayerDevice::create();
    var player = playerDevice->player();
    SynthAdapter& sa = hostApp()->synthAdapter();
    player->addAdapter(&sa);
    player->initialize();

    // 1. create a device for every type defined in the adapter
    for (var ti=0; ti<255; ti++) {
        var dev = player->addDevice(&sa, ti);
        if (dev != NULL) {
            char id[16];
            sprintf_s(id, 16, "chn%02X", ti);
            player->addChannel(id);
        }
    }

    // 2.a create master sequence
    var masterSeq = NEW_(Sequence, playerDevice);
    masterSeq->writeHeader();
    masterSeq->writeDelta(0);
    for (var di = 1; di < player->devices().length(); di++) {
        masterSeq->writeCommand(PlayerCommands::CmdAssign)->writeByte(di)->writeByte(di)->writeByte(di)->writeByte(1);
    }
    player->addSequence(masterSeq);


    // 2.b create the default sequence for each device
    var length = 64;
    for (var di = 1; di < player->devices().length(); di++) {
        var dev = (Device*)player->devices().get(di);
        var seq = dev->createDefaultSequence();
        //if (length < seq->frameCount()) length = seq->frameCount();
        player->addSequence(seq);
    }

    masterSeq->writeEOF();
    masterSeq->writeDelta(length);
    masterSeq->writeEOS();
    player->refreshRate(16.0f);

    return playerDevice;

//
//
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
//    initData->writeFloat(4 * 24.0f);
//    // 5 channels
//    initData->writeByte(5);
//
//#pragma region Sequences
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
//#pragma endregion
//
//#pragma region Adapters
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
//#pragma endregion
//
//#pragma region CreateStream
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
//#pragma endregion
//
//    for (var di = 0; di < count; di++) {
//        DEL_(devices[di]);
//    }
//
//    return data;

}

PlayerDevice* FileManager::loadBinary(const char* path) {
    PlayerDevice* playerDevice = NULL;
    byte* data = NULL;
    var bytesRead = File::read(path, &data);
    if (bytesRead > 0) {
        var playerDevice = PlayerDevice::create();
        var ptr = &data;
        playerDevice->initialize(ptr);
    }
    return playerDevice;
}

int FileManager::saveBinary(const char* path, PlayerDevice* playerDevice) {
    int res = 0;
    var data = playerDevice->player()->save();
    res = (int)data->writeBytesToFile(path);
    DEL_(data);
    return 0;
}

#pragma endregion

#pragma region Module
PlayerDevice* FileManager::loadModule(const char* path) {
	// load XM module and create binary data
    var masterDevice = PlayerDevice::create();
    var player = masterDevice->player();
    var soundbanks = loadSoundbanks("soundbanks.bin");
    var xmLoader = NEW_(XmLoader, masterDevice);
    xmLoader->load(path, soundbanks);
    // create default mixer
    var mixerDevice = (MixerDevice*)player->addDevice(&hostApp()->synthAdapter(), SynthDevices::DeviceMixer);
    var mixer = (Mixer8x4*)mixerDevice->module();
    mixer->channelCount(player->devices().length() - 2);
    for (var di = 0; di < mixer->channelCount(); di++) {
        var channel = mixer->getChannel(di);
        mixer->connectInput(channel, ((ModuleDevice*)player->devices().get(di + 1))->module());
        channel->controls->amp.value.f = 0.3f;
        channel->controls->gain.value.f = 0.8f;
        channel->controls->pan.value.f = 0.5f;
        channel->stageCount = 0;
    }
    DEL_(xmLoader);

    return masterDevice;
}

#pragma endregion

#pragma region Soundbank
Map* FileManager::loadSoundbanks(const char* path) {
    var fileName = path != NULL ? path : "soundbanks.bin";
	var soundbanks = ModuleDevice::loadSoundbanks(fileName, &hostApp()->synthAdapter());
	return soundbanks;
}

int FileManager::saveSoundbanks(Map* soundbanks, const char* path) {
    int res = 0;
    Stream stream;
    for (var i = 0; i < soundbanks->size(); i++) {
        stream.writeByte(*(int*)soundbanks->keys()->get(i));
        var sb = (Soundbank*)soundbanks->values()->get(i);
        sb->writeToStream(&stream);
        DEL_(sb);
    }
    var fileName = path != NULL ? path : "soundbanks.bin";
    res = (int)stream.writeBytesToFile(fileName);

    return res;
}
#pragma endregion