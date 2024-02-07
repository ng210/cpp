#ifndef _PLAYERTEST_H
#define _PLAYERTEST_H

#include "test.h"
#include "player/src/player-adapter.h"
#include "player/src/player-device-ext.h"
#include "player/test/cons-device-ext.h"

using namespace PLAYER;

class PlayerTest : public Test {
    PlayerAdapter playerAdapter_;
    ConsAdapter consAdapter_;
    PlayerDevice* playerDevice_;
    PlayerDeviceExt* playerDeviceExt_;
    ConsDevice* consDevice_;
    ConsDeviceExt* consDeviceExt_;
    byte* binaryData;
    Value value1_, value2_;

    static HANDLER_FUNC(onSetValue, Value);

    void createSequences(Player* player);
    Stream* createBinaryData();

    int preset_;
    static HANDLER_FUNC(onSetPreset, int);
    PresetBank* presetBank_;
    static HANDLER_FUNC(onSetPresetBank, PresetBank*);
public:
    // Inputs
    void testInput();
    void testInputF();
    void testInputF8();

    // Create Player
    void testCreatePlayer();
    void testCreatePlayerExt();

    // ConsDevice
    void testCreateConsDevice();
    void testCreateConsDeviceExt();
    void testCreateConsDeviceExtSetters();

    // Sequence and Frames
    void testCreateSequence();
    void testCreateAndEditFrames();
    void testCreateFramesFromSequence();
    void testCreateSequenceFromFrames();

    // Binary
    void testLoadFromBinary();
    void testCreateBinary();

    // Playback
    void testRunChannelFromSequence();
    void testRunChannelFromFrames();
    void testRunPlayerOnSequence();
    void testRunPlayerOnFrames();

    void runAll(int& totalPassed, int& totalFailed);

    static int LAST_TICK;
};

#endif