#ifndef SYNTH_TEST_H
#define SYNTH_TEST_H

#include "soundlib/src/soundplayer.h"
#include "test.h"
#include "player/src/player-ext.h"
#include "player/src/player-device.h"
#include "player/src/player-device-ext.h"
#include "synth/src/module/module.h"
#include "synth/src/device/bass-device-ext.h"
#include "synth/src/device/synth-device-ext.h"
#include "synth/src/device/generic-drum-device-ext.h"
#include "synth/src/device/mixer-device-ext.h"


NS_FW_BASE_USE
using namespace SYNTH;
using namespace PLAYER;

typedef int (*PlayCallback)(Module*, int, void*);

class SynthTest : public Test {
    Player* player_;
    PlayerExt* playerExt_;
    PlayerDevice* masterDevice_;
    PlayerDeviceExt* masterDeviceExt_;
    SynthAdapter* synthAdapter_;

    static void simpleSynthCallback(short* buffer, int sampleCount, void* context);
    void playback(Module* mdl, PlayCallback callback, float fps, int chn, char* filePath = NULL, FeedSample feedSample = NULL, void* args = NULL);
    bool testInput(InputBase& input, Value min, Value max, Value Step, Value* value);
    int testEnvelope(Env& env, short* buffer, void(callback)(int, Env&, float), char* path);
    void playDefaultSequence(SynthBaseDevice* dev, SynthBaseDeviceExt* ext);
public:
    float samplingRate;
    bool saveToFile;

    SynthTest();
    ~SynthTest();

    void testEnvelopes();
    void testLfo();
    //void testFilterSound();

    void testBass();
    void testBassDeviceExt();
    void testSynth1();
    void testSynth1DeviceExt();
    //void testSynth2();
    //void testSynth2Device();
    //DrumsDevice* setupDrums();
    void testGenericDrum();
    //void testDrums();
    //void testDrumsDevice();
    void testMixer();
    void testMixerDeviceExt();

    void runAll(int& totalPassed, int& totalFailed);    
};

#endif