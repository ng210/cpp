#ifndef SYNTH_TEST_H
#define SYNTH_TEST_H

#include "test.h"
#include "soundlib/src/soundplayer.h"
#include "soundlib/src/wavfmt.h"
#include "player/src/player-lib.h"
#include "player/src/player-adapter.h"
#include "synth/src/device/synth-adapter.h"
#include "synth/src/device/bass-device.h"
#include "synth/src/device/synth-device.h"
#include "synth/src/device/drums-device.h"
#include "synth/src/device/mixer-device.h"


NS_FW_BASE_USE
using namespace SYNTH;
using namespace PLAYER;

typedef int (*PlayCallback)(Module*, int);

class SynthTest : public Test {
    Player* player_;
    PlayerDevice* masterDevice_;
    SynthAdapter* synthAdapter_;

    static void simpleSynthCallback(short* buffer, int sampleCount, void* context);
    void playback(Module* mdl, PlayCallback callback, float fps, char* filePath = NULL);
    int testEnvelope(Env* env, byte* settings, int size, short* buffer, char* path = NULL);
public:
    float samplingRate;
    bool saveToFile;

    SynthTest();
    ~SynthTest();

    void testEnvelopes();
    //void testFilterSound();

    void testBass();
    BassDevice* setupBass();
    void testBassDevice();
    void testSynth1();
    void testSynth1Device();
    void testSynth2();
    void testSynth2Device();
    void testDrums();
    DrumsDevice* setupDrums();
    void testDrumsDevice();
    void testMixer();

    void runAll(int& totalPassed, int& totalFailed);    
};

#endif