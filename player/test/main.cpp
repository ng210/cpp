#include "console/consoleapp.h"
#include "test.h"
#include "player/src/player-lib.h"
#include "player/src/player-adapter.h"
#include "player/test/cons-adapter.h"
#include "player/test/cons-device.h"
#include "player/src/frame-list.h"

NS_FW_BASE_USE
using namespace PLAYER;

#define LAST_TICK 120

#pragma region classPlayerTest
class PlayerTest : public Test {

    static PlayerAdapter playerAdapter_;
    static ConsAdapter consAdapter_;

    Stream* createBinaryData();

public:
    PlayerTest() { }
    void testCreatePlayer();
    void testCreateSequence();
    void testCreateAndEditFrames();
    void testCreateFramesFromSequence();
    void testCreateSequenceFromFrames();
    void testLoadFromBinary();
    void testCreateBinary();
    void testRunChannelFromSequence();
    void testRunChannelFromFrames();
    void testRunPlayerOnSequence();
    void testRunPlayerOnFrames();
    //int testPrintSequence();

    void runAll(int& totalPassed, int& totalFailed);
};

PlayerAdapter PlayerTest::playerAdapter_;
ConsAdapter PlayerTest::consAdapter_;

void createSequences(Player* player) {
    // sequence #1
    var sequence1 = NEW_(Sequence);
    {
        sequence1->writeByte(0);    // #0 device: PlayerDevice
        // Frame #1 - 8
        sequence1->writeDelta(0);
        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(1)->writeByte(0);
        sequence1->writeEOF();
        // Frame #2 - 3
        sequence1->writeDelta(LAST_TICK);
        sequence1->writeEOS();

        player->addSequence(sequence1);
    }
    // sequence #2
    var sequence2 = NEW_(Sequence);
    {
        sequence2->writeByte(1);    // #1 device: ConsDevice
        // Frame #1 - 14 bytes
        
        // Frame #1
        sequence2->writeDelta(0);
        sequence2->writeCommand(ConsCommands::CmdMoveTo)->writeByte(0)->writeByte(1);
        sequence2->writeCommand(ConsCommands::CmdSetText)->writeString("Seq1.1");
        sequence2->writeEOF();
        // Frame #2
        sequence2->writeDelta(20);
        sequence2->writeCommand(ConsCommands::CmdMoveTo)->writeByte(-6)->writeByte(0);
        sequence2->writeCommand(ConsCommands::CmdSetText)->writeString("Seq1.2");
        sequence2->writeCommand(ConsCommands::CmdSetInk)->writeByte(1);
        sequence2->writeEOF();
        // Frame #3
        sequence2->writeDelta(20);
        sequence2->writeCommand(ConsCommands::CmdMoveTo)->writeByte(-6)->writeByte(0);
        sequence2->writeCommand(ConsCommands::CmdSetText)->writeString("Seq1.3");
        sequence2->writeCommand(ConsCommands::CmdSetInk)->writeByte(2);
        sequence2->writeEOF();
        // Frame #4
        sequence2->writeDelta(20);
        sequence2->writeCommand(ConsCommands::CmdMoveTo)->writeByte(-6)->writeByte(0);
        sequence2->writeCommand(ConsCommands::CmdSetText)->writeString("Seq1.4");
        sequence2->writeCommand(ConsCommands::CmdSetInk)->writeByte(3);
        sequence2->writeEOF();
        // Frame #5
        sequence2->writeDelta(20);
        sequence2->writeCommand(ConsCommands::CmdMoveTo)->writeByte(-6)->writeByte(1);
        sequence2->writeCommand(ConsCommands::CmdSetText)->writeString("End");
        sequence2->writeCommand(ConsCommands::CmdSetInk)->writeByte(4);
        sequence2->writeEOF();
        // Frame #6
        sequence2->writeDelta(10);
        sequence2->writeCommand(ConsCommands::CmdSetInk)->writeByte(7);
        sequence2->writeEOF();
        // Frame #7
        sequence2->writeDelta(10);
        sequence2->writeCommand(ConsCommands::CmdMoveTo)->writeByte(-3)->writeByte(0);
        sequence2->writeCommand(ConsCommands::CmdSetText)->writeString("End\n");
        sequence2->writeEOS();

        player->addSequence(sequence2);
    }
}

Stream* toCommandStream(FrameList* frames) {
    var merge = NEW_(Stream);
    for (var fi = 0; fi < frames->length(); fi++) {
        var frame = (Frame*)frames->get(fi);
        for (var ci = 0; ci < frame->commands.length(); ci++) {
            var cmd = (byte*)frame->commands.get(ci);
            var len = frames->device()->getCommandSize(cmd);
            merge->writeBytes(cmd, len);
        }
    }
    return merge;
}

Stream* PlayerTest::createBinaryData() {
    var data = NEW_(Stream);

    var initData = NEW_(Stream);
    initData->writeFloat(25.0f);
    // 1 channel
    initData->writeByte(1);

    Player player;
    createSequences(&player);
    var sequence1 = (Sequence*)player.sequences().get(0);
    var sequence2 = (Sequence*)player.sequences().get(1);

    var adapterList = NEW_(Stream);
    // 2 adapters
    adapterList->writeByte(2);
    // 1st adapter: PlayerAdapter
    {
        adapterList->writeByte(PlayerTest::playerAdapter_.getInfo()->id);
        // no init data
        // no devices
        adapterList->writeByte(0);
    }
    // 2nd adapter: ConsAdapter
    {
        adapterList->writeByte(PlayerTest::consAdapter_.getInfo()->id);
        adapterList->writeDword(0x12345678);
        // 1 device
        adapterList->writeByte(1);
        adapterList->writeByte(ConsDevices::DeviceCons);
        adapterList->writeByte(2);
        adapterList->writeByte(1);
        adapterList->writeByte(ConsoleColors::cyan);
    }

    var sequenceList = NEW_(Stream);
    sequenceList->writeByte(2);
    sequenceList->writeWord((word)sequence1->length());
    sequenceList->writeWord((word)sequence2->length());
    sequenceList->writeStream(sequence1);
    sequenceList->writeStream(sequence2);

    // header
    // 3 data blocks
    data->writeWord(3);
    data->writeDword(initData->length());
    data->writeDword(adapterList->length());
    data->writeDword(sequenceList->length());

    data->writeStream(initData);
    DEL_(initData);
    data->writeStream(adapterList);
    DEL_(adapterList);
    data->writeStream(sequenceList);
    DEL_(sequenceList);

    return data;
}

void PlayerTest::testCreatePlayer() {
    var device = PlayerDevice::create(NULL);
    var adapter = Player::getAdapter(playerAdapter_.getInfo()->id);
    assert("Should add Player-Adapter", adapter != NULL && adapter->getInfo()->id == PlayerTest::playerAdapter_.getInfo()->id);
    assert("Should create a Player-Device", device != NULL);
    assert("Should create wrapped Player object", device->object() != NULL);
    var player = device->player();
    assert("Should have a master Channel", player->masterChannel() != NULL && fmw::strncmp(player->masterChannel()->id(), "master") == 0);
    DEL_(device);
    Player::cleanUp();
}

void PlayerTest::testCreateSequence() {
    var device = PlayerDevice::create(NULL);
    var player = device->player();
    createSequences(player);
    assert("Should have 1 sequence", player->sequences().length() == 2);
    assert("Should have 12 bytes", ((Sequence*)player->sequences().get(0))->length() == 1+2+5+1+2+1);
    DEL_(device);
    Player::cleanUp();
}

void PlayerTest::testCreateAndEditFrames() {
    Player::addAdapter(NEW_(ConsAdapter));
    var device = PlayerDevice::create(NULL);
    var player = device->player();
    byte initData[] = { 0, 0, gray };
    byte* pData = initData;
    var consDevice = player->addDevice(&PlayerTest::consAdapter_, DeviceCons, (byte**)&pData);
    byte commands[] = {
        ConsCommands::CmdSetInk, red,
        ConsCommands::CmdMoveTo, 0, 2,
        ConsCommands::CmdSetText, 'H', 'e', 'l', 'l', 'o', '!', '\0',
        PlayerCommands::CmdEOF,

        ConsCommands::CmdSetInk, gray,
        ConsCommands::CmdMoveTo, 0, (byte)-6,
        ConsCommands::CmdSetText, 'H', 'e', 'l', 'l', 'o', '!', '\0',
        PlayerCommands::CmdEOF,

        ConsCommands::CmdSetInk, black,
        ConsCommands::CmdMoveTo, 0, (byte)-6,
        ConsCommands::CmdSetText, 'H', 'e', 'l', 'l', 'o', '!', '\0',
        PlayerCommands::CmdEOF,

        ConsCommands::CmdSetInk, yellow,
        ConsCommands::CmdMoveTo, 0, (byte)-6,
        ConsCommands::CmdSetText, 'B', 'y', 'e', '!', '\0',
        PlayerCommands::CmdEOF,

        ConsCommands::CmdSetInk, yellow,
        ConsCommands::CmdMoveTo, 0, (byte)-4,
        ConsCommands::CmdSetText, 'B', 'y', 'e', '!', '\0',
        PlayerCommands::CmdEOF,

        ConsCommands::CmdSetInk, black,
        ConsCommands::CmdMoveTo, 0, (byte)-4,
        ConsCommands::CmdSetText, 'B', 'y', 'e', '!', '\0',
        PlayerCommands::CmdEOS
    };

    var frames = NEW_(FrameList, consDevice);
    var time = 0;
    byte* cmd = (byte*)&commands;
    for (var ii = 0; ii < arraysize(commands); ii++) {
        var len = frames->insertCommand(time, cmd);
        if (*cmd == PlayerCommands::CmdEOF) time += 10;
        else if (*cmd == PlayerCommands::CmdEOS) break;
        cmd += len;
    }

    commands[1] = yellow;
    frames->insertCommand(0, commands); // overwrite command

    var json = frames->toJSON();
    LOG("Frames as JSON\n%s\n", json);
    FREE(json);

    assert("Should have 6 frames", frames->length() == 6);
    // merge all commands into 1 stream
    var merge = toCommandStream(frames);
    int ix = -1;
    assert("Should have correct commands", memcmp(merge->data(), commands, merge->length(), ix) == 1);
    DUMP(commands, sizeof(commands), 32);
    DUMP(merge->data(), merge->length(), 32);
    DEL_(merge);

    frames->removeCommand(0, &commands[0]);
    var frame = (Frame*)frames->get(0);
    assert("Should have 1 less commands in frame #1", frame->commands.length() == 3);
    merge = toCommandStream(frames);
    DUMP(merge->data(), merge->length(), 32);
    DEL_(merge);

    DEL_(frames);
    DEL_(device);
    Player::cleanUp();
}

void PlayerTest::testCreateFramesFromSequence() {
    Player::addAdapter(NEW_(ConsAdapter));
    var device = PlayerDevice::create(NULL);
    var player = device->player();
    byte initData[] = { 0, 0, 0 };
    byte* pData = initData;
    var consDevice = player->addDevice(&PlayerTest::consAdapter_, DeviceCons, (byte**) & pData);
    createSequences(device->player());
    var seq = (Sequence*)device->player()->sequences().get(1);

    var frames = FrameList::fromSequence(seq);
    assert("Should have 7 frames", frames->length() == 7);
    var frame = (Frame*)frames->get(0);
    assert("Frame #1 should have correct commands",
        frame->commands.length() == 3 && frame->time == 0 &&
        *(byte*)frame->commands.get(0) == ConsCommands::CmdMoveTo &&
        *(byte*)frame->commands.get(1) == ConsCommands::CmdSetText &&
        *(byte*)frame->commands.get(2) == PlayerCommands::CmdEOF);
    frame = (Frame*)frames->get(6);
    assert("Frame #7 should have correct commands",
        frame->commands.length() == 3 && frame->time == 100 &&
        *(byte*)frame->commands.get(0) == ConsCommands::CmdMoveTo &&
        *(byte*)frame->commands.get(1) == ConsCommands::CmdSetText &&
        *(byte*)frame->commands.get(2) == PlayerCommands::CmdEOS);

    var json = frames->toJSON();
    LOG("Frames as JSON\n%s\n", json);
    FREE(json);
    DEL_(frames);
    DEL_(device);
    Player::cleanUp();
}

void PlayerTest::testCreateSequenceFromFrames() {
    Player::addAdapter(NEW_(ConsAdapter));
    var device = PlayerDevice::create(NULL);
    var player = device->player();
    byte initData[] = { 0, 0, 0 };
    byte* pData = initData;
    var consDevice = player->addDevice(&PlayerTest::consAdapter_, DeviceCons, (byte**)&pData);
    createSequences(device->player());
    var seq1 = (Sequence*)device->player()->sequences().get(1);
    var frames = FrameList::fromSequence(seq1);

    var seq2 = frames->toSequence(seq1->device());
    int pos = -1;
    assert("Should match original sequence",
        seq1->length() == seq2->length() &&
        fmw::memcmp(seq1->data(), seq2->data(), seq1->length(), pos) != false &&
        pos == seq1->length());
    LOG("Sequence #1\n");
    DUMP(seq1->data(), seq1->length(), 32);
    LOG("Sequence #2\n");
    DUMP(seq1->data(), seq2->length(), 32);

    DEL_(seq2);
    DEL_(frames);
    DEL_(device);
    Player::cleanUp();
}

void PlayerTest::testLoadFromBinary() {
    var bin = createBinaryData();
    Player::addAdapter(NEW_(ConsAdapter));
    var cons = getConsole();
    cons->dump(bin->data(), bin->length(), 16);
    var data = bin->extract();
    DEL_(bin);
    var device = PlayerDevice::create(&data);
    var player = device->player();

    assert("Should have 2 channels", player->channels().length() == 2);
    assert("Should have 25.0 fps", player->refreshRate() == 25.0f);
    assert("Should have PlayerDevice as #1", ((Device*)player->devices().get(0))->type() == PlayerDevices::DevicePlayer);
    assert("Should have ConsDevice as #2", ((Device*)player->devices().get(0))->type() == ConsDevices::DeviceCons);
    assert("Should have 2 sequences", player->sequences().length() == 2);

    DEL_(device);
    Player::cleanUp();
}

void PlayerTest::testCreateBinary() {
    var bin = createBinaryData();
    Player::addAdapter(NEW_(ConsAdapter));
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

void PlayerTest::testRunChannelFromSequence() {
    Player::addAdapter(NEW_(ConsAdapter));
    var device = PlayerDevice::create(NULL);
    var player = device->player();
    byte initData[] = { 0, 0, gray };
    byte* pData = initData;
    var consDevice = player->addDevice(&PlayerTest::consAdapter_, DeviceCons, (byte**)&pData);
    var ch = player->addChannel("Chn1");
    createSequences(player);
    ch->assign(consDevice, (Sequence*)player->sequences().get(1));

    while (ch->isActive()) {
        ch->run(1);
        Sleep(10);
    }

    assert("Should run to the end", ch->loopCount() == -1);

    DEL_(device);
    Player::cleanUp();
}

void PlayerTest::testRunChannelFromFrames() {
    Player::addAdapter(NEW_(ConsAdapter));
    var device = PlayerDevice::create(NULL);
    var player = device->player();
    byte initData[] = { 0, 0, gray };
    byte* pData = initData;
    var consDevice = player->addDevice(&PlayerTest::consAdapter_, DeviceCons, (byte**)&pData);
    var ch = player->addChannel("Chn1");
    createSequences(player);
    var seq = (Sequence*)device->player()->sequences().get(1);
    var frames = FrameList::fromSequence(seq);
    ch->assign(consDevice, frames);

    while (ch->isActive()) {
        ch->run(1);
        Sleep(10);
    }

    assert("Should run to the end", ch->loopCount() == -1);

    DEL_(frames);
    DEL_(device);
    Player::cleanUp();
}

void PlayerTest::testRunPlayerOnSequence() {
    var bin = createBinaryData();
    Player::addAdapter(NEW_(ConsAdapter));
    var data = bin->extract();
    DEL_(bin);
    var device = PlayerDevice::create(&data);
    var player = device->player();
    for (var si = 0; si < player->sequences().length(); si++) {
        var seq = (Sequence*)player->sequences().get(si);
        var frames = FrameList::fromSequence(seq);
        player->frameLists().push(frames);
    }
    device->mode(PlayerModeFrameList);
    player->useThread();
    player->start();
    while (device->isActive()) {
        Sleep(10);
    }
    assert("Should run to the end", ((Channel*)player->channels().get(0))->loopCount() == -1);
    
    DEL_(device);
    Player::cleanUp();
}

void PlayerTest::testRunPlayerOnFrames() {
    var bin = createBinaryData();
    Player::addAdapter(NEW_(ConsAdapter));
    var data = bin->extract();
    DEL_(bin);
    var device = PlayerDevice::create(&data);
    var player = device->player();
    player->useThread();
    player->start();
    while (device->isActive()) {
        Sleep(10);
    }
    assert("Should run to the end", ((Channel*)player->channels().get(0))->loopCount() == -1);

    DEL_(device);
    Player::cleanUp();
}


void PlayerTest::runAll(int& totalPassed, int& totalFailed) {
    //test("Create Player", (TestMethod)&PlayerTest::testCreatePlayer);
    //test("Create Sequence", (TestMethod)&PlayerTest::testCreateSequence);
    //test("Create and edit FrameList", (TestMethod)&PlayerTest::testCreateAndEditFrames);
    //test("Create FrameList from Sequence", (TestMethod)&PlayerTest::testCreateFramesFromSequence);
    //test("Create Sequence from FrameList", (TestMethod)&PlayerTest::testCreateSequenceFromFrames);
    //test("Load from binary", (TestMethod)&PlayerTest::testLoadFromBinary);
    //test("Create binary", (TestMethod)&PlayerTest::testCreateBinary);
    test("Run channel from sequence", (TestMethod)&PlayerTest::testRunChannelFromSequence);
    test("Run channel from frames", (TestMethod)&PlayerTest::testRunChannelFromFrames);
    test("Run Player on sequence", (TestMethod)&PlayerTest::testRunPlayerOnSequence);
    test("Run Player on frames", (TestMethod)&PlayerTest::testRunPlayerOnFrames);
    
    //TEST(testPrintSequence); 
    displayFinalResults();
}

#pragma endregion

int _main(NS_FW_BASE::Map* args) {
#ifdef _DEBUG
    Debug::initialize(/*DEBUG_UNICODE | DEBUG_MEMORY*/);
#endif

    var tests = NEW_(PlayerTest);
    LOG("Player tests\n");
    int totalPassed, totalFailed;
    tests->runAll(totalPassed, totalFailed);
    DEL_(tests);

    return 0;
}