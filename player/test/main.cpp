#include "console/consoleapp.h"
#include "test.h"
#include "player/src/player-lib.h"
#include "player/src/player-adapter.h"
#include "player/test/cons-adapter.h"
#include "player/test/cons-device.h"
#include "player/src/frames.h"

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
    void testCreateFramesFromSequence();
    void testCreateSequenceFromFrames();
    void testLoadFromBinary();
    void testCreateBinary();
    void testRunPlayer();
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

Frames* createFrames(Device* device) {
    createSequences(device->player());
    var seq = (Sequence*)device->player()->sequences().get(1);
    var frames = Frames::fromSequence(seq,
        [](UINT8*& cursor) {
            var channelId = 0;
            var cmdId = *cursor;
            switch (cmdId) {
                case PlayerCommands::CmdEOS:
                case PlayerCommands::CmdEOF:
                    channelId = cmdId;
                    break;
                default:
                    channelId = cmdId;
            }

            return channelId;
        }
    );

    return frames;
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

void PlayerTest::testCreateFramesFromSequence() {
    Player::addAdapter(NEW_(ConsAdapter));
    var device = PlayerDevice::create(NULL);
    var player = device->player();
    byte initData[] = { 0, 0, 0 };
    byte* pData = initData;
    var consDevice = player->addDevice(&PlayerTest::consAdapter_, DeviceCons, (byte**) & pData);
    var frames = createFrames(device);

    assert("Should create 3 channels", frames->channels().size() == 3);
    assert("Should have 6 SetText commands", ((Array*)frames->channels().get(ConsCommands::CmdSetText))->length()  == 6);
    assert("Should have 5 MoveTo commands", ((Array*)frames->channels().get(ConsCommands::CmdMoveTo))->length()  == 5);
    assert("Should have 5 SetInk commands", ((Array*)frames->channels().get(ConsCommands::CmdSetInk))->length()  == 5);

    byte cmds[] = {
        ConsCommands::CmdMoveTo, 0, 1,
        ConsCommands::CmdSetInk, 2,
        ConsCommands::CmdMoveTo, 0, 0xff,
        ConsCommands::CmdSetInk, 1,
    };

    byte* pCmd = cmds;
    frames->insertCommand(10, *pCmd, pCmd); pCmd += 3;
    frames->insertCommand(10, *pCmd, pCmd); pCmd += 2;
    frames->insertCommand(30, *pCmd, pCmd); pCmd += 3;
    frames->insertCommand(30, *pCmd, pCmd); pCmd += 2;

    var chn = (Array*)frames->channels().get(ConsCommands::CmdMoveTo);
    Key p1 = -1, p2 = -1;
    var cmd1 = (Command*)chn->binSearch(10, p1, Frames::compareCommand);
    var cmd2 = (Command*)chn->binSearch(30, p2, Frames::compareCommand);
    assert("Should insert 2 MoveTo commands",
        chn->length() == 7 &&
        memcmp(cmd1->command, &cmds[0], 3) == 0 &&
        memcmp(cmd2->command, &cmds[5], 3) == 0
    );

    chn = (Array*)frames->channels().get(ConsCommands::CmdSetInk);
    cmd1 = (Command*)chn->binSearch(10, p1, Frames::compareCommand);
    cmd2 = (Command*)chn->binSearch(30, p2, Frames::compareCommand);
    assert("Should insert 2 SetInk commands",
        chn->length() == 7 &&
        memcmp(cmd1->command, &cmds[3], 2) == 0 &&
        memcmp(cmd2->command, &cmds[8], 2) == 0
    );

    for (var chi = 0; chi < frames->channels().keys()->length(); chi++) {
        var channelId = *(int*)frames->channels().keys()->get(chi);
        INFO("\nChannel #%0d ----------------\n", channelId);
        chn = (Array*)frames->channels().get(channelId);
        for (var ci = 0; ci < chn->length(); ci++) {
            var cmd = (Command*)chn->get(ci);
            INFO("#%03d: ", cmd->time);
            var size = consDevice->getCommandSize(*cmd->command, cmd->command + 1);
            DUMP(cmd->command, size, 16);
        }
    }

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
    var frames = createFrames(device);
    var seq1 = frames->toSequence();
    var seq2 = (Sequence*)player->sequences().get(1);
    var isMatching = seq1->length() == seq2->length() && memcmp(seq1->data(), seq2->data(), seq1->length()) == 0;
    assert("Should create the same sequence", isMatching);
    if (!isMatching) {
        DUMP(seq1->data(), seq1->length(), 16);
        DUMP(seq2->data(), seq2->length(), 16);
    }
    DEL_(seq1);
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

void PlayerTest::testRunPlayer() {
    var bin = createBinaryData();
    Player::addAdapter(NEW_(ConsAdapter));
    var data = bin->extract();
    DEL_(bin);
    var device = PlayerDevice::create(&data);
    var player = device->player();
    player->useThread();
    player->start();
    while (device->isActive()) {
        Sleep(6);
    }
    assert("Should run to the end", true);
    
    DEL_(device);
    Player::cleanUp();
}

void PlayerTest::runAll(int& totalPassed, int& totalFailed) {
    test("Create Player", (TestMethod)&PlayerTest::testCreatePlayer);
    test("Create Sequence", (TestMethod)&PlayerTest::testCreateSequence);
    test("Create Frames from Sequence", (TestMethod)&PlayerTest::testCreateFramesFromSequence);
    test("Create Sequence from Frames", (TestMethod)&PlayerTest::testCreateSequenceFromFrames);
    test("Load from binary", (TestMethod)&PlayerTest::testLoadFromBinary);
    test("Create binary", (TestMethod)&PlayerTest::testCreateBinary);
    test("Run Player", (TestMethod)&PlayerTest::testRunPlayer);
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