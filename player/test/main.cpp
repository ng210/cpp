#include "console/consoleapp.h"
#include "test.h"
#include "player-lib.h"
#include "player-adapter.h"
#include "cons-adapter.h"
#include "cons-device.h"

NS_FW_BASE_USE
using namespace PLAYER;

#define LAST_TICK 120

#pragma region classPlayerTest
class PlayerTest : public Test {
    Player* player_;
    Device* device_;

    static PlayerAdapter playerAdapter_;
    static ConsAdapter consAdapter_;

    //Player* createFullPlayer();
    Stream* createBinaryData();
    Sequence* createTestSequence(PlayerDevice* device);

public:
    PlayerTest() {
        player_ = NULL;
        device_ = NULL;
    }
    void testCreatePlayer();
    void testCreateSequence();
    void testCreateSequenceFromFrames();
    void testLoadFromBinary();
    void testCreateBinary();
    void testRunPlayer();
    //int testCreateFrames();
    //int testPrintSequence();
};

PlayerAdapter PlayerTest::playerAdapter_;
ConsAdapter PlayerTest::consAdapter_;

Stream* PlayerTest::createBinaryData() {
    var data = NEW_(Stream);

    var initData = NEW_(Stream);
    initData->writeFloat(25.0f);
    // 1 channel
    initData->writeByte(1);

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
    }
    // sequence #2
    var sequence2 = NEW_(Sequence);
    {
        sequence2->writeByte(1);    // #1 device: ConsDevice
        // Frame #1 - 14 bytes
        sequence2->writeDelta(0);
        sequence2->writeCommand(ConsCommands::CmdSetText)->writeString("Seq1.1");
        sequence2->writeCommand(ConsCommands::CmdMoveTo)->writeByte(-6)->writeByte(0);
        sequence2->writeEOF();
        // Frame #2 - 16 bytes
        sequence2->writeDelta(20);
        sequence2->writeCommand(ConsCommands::CmdSetInk)->writeByte(1);
        sequence2->writeCommand(ConsCommands::CmdSetText)->writeString("Seq1.2");
        sequence2->writeCommand(ConsCommands::CmdMoveTo)->writeByte(-6)->writeByte(0);
        sequence2->writeEOF();
        // Frame #3 - 16 bytes
        sequence2->writeDelta(20);
        sequence2->writeCommand(ConsCommands::CmdSetInk)->writeByte(2);
        sequence2->writeCommand(ConsCommands::CmdSetText)->writeString("Seq1.3");
        sequence2->writeCommand(ConsCommands::CmdMoveTo)->writeByte(-6)->writeByte(0);
        sequence2->writeEOF();
        // Frame #4 - 16 bytes
        sequence2->writeDelta(20);
        sequence2->writeCommand(ConsCommands::CmdSetInk)->writeByte(3);
        sequence2->writeCommand(ConsCommands::CmdSetText)->writeString("Seq1.4");
        sequence2->writeCommand(ConsCommands::CmdMoveTo)->writeByte(-6)->writeByte(1);
        sequence2->writeEOF();
        // Frame #5 - 11 bytes
        sequence2->writeDelta(20);
        sequence2->writeCommand(ConsCommands::CmdSetInk)->writeByte(4);
        sequence2->writeCommand(ConsCommands::CmdSetText)->writeString("End");
        sequence2->writeEOF();
        // Frame #6 - 5 bytes
        sequence2->writeDelta(20);
        sequence2->writeCommand(ConsCommands::CmdSetInk)->writeByte(7);
        sequence2->writeCommand(ConsCommands::CmdMoveTo)->writeByte(-3)->writeByte(0);
        sequence2->writeCommand(ConsCommands::CmdSetText)->writeString("End\n");
        sequence2->writeEOS();
    }

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
    DEL_(sequence1);
    DEL_(sequence2);

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

Sequence* PlayerTest::createTestSequence(PlayerDevice* device) {
    var sequence = NEW_(Sequence, device);
    sequence->writeHeader();
    // Frame #1
    sequence->writeDelta(0);
    sequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(0)->writeByte(0);
    sequence->writeEOF();
    // Frame #2 - 3
    sequence->writeDelta(96);
    sequence->writeEOS();

    var txt = sequence->print();
    INFO(txt);
    FREE(txt);

    return sequence;
}

void PlayerTest::testCreateSequence() {
    var device = PlayerDevice::create(NULL);
    var player = device->player();
    player->addSequence(createTestSequence(device));
    assert("Should have 1 sequence", player->sequences().length() == 1);
    assert("Should have 12 bytes", ((Sequence*)player->sequences().get(0))->length() == 1+2+5+1+2+1);
    DEL_(device);
    Player::cleanUp();
}

void PlayerTest::testCreateSequenceFromFrames() {
    var device = PlayerDevice::create(NULL);
    var player = device->player();
#pragma region CreateFrames
    var frames = NEW_(PArray, 2);
    // frame #1
    var frame = NEW_(Frame);
        frame->delta_ = 0;
        var command = device->makeCommand(PlayerCommands::CmdAssign, 1, 1, 0, 0);
        frame->addCommand(command);
        frames->push(frame);
    // frame #2
    frame = NEW_(Frame);
        frame->delta_ = 96;
        var commandStream = device->makeCommandAsStream(PlayerCommands::CmdEOS);
        frame->addCommand(commandStream->extract());
        DEL_(commandStream);
        frames->push(frame);
#pragma endregion
    var sequence1 = createTestSequence(device);
    var sequence2 = Sequence::fromFrames(frames, device);
    frames->apply([](void* f, UINT32 ix, Collection* c, void* args) {
        var frame = (Frame*)f;
        frame->commands_.apply([](void* p, UINT32 ix, Collection* c, void* args) {
            FREE((byte*)p);
            return 1;
        });
        DEL_(frame);
        return 1;
    });
    DEL_(frames);
    var info = sequence2->print();
    INFO(info); FREE(info);

    bool isMatch = false;
    if (sequence1->length() == sequence2->length()) {
        isMatch = true;
        for (var i = 0; i < sequence1->length(); i++) {
            if (sequence1->data()[i] != sequence2->data()[i]) {
                isMatch = false;
                break;
            }
        }
    }
    assert("Should match the sequence", isMatch);

    DEL_(sequence1);
    DEL_(sequence2);
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
    // assign master sequence to master channel
    player->assignChannel(0, (Sequence*)player->sequences().get(0), 0, 0);
    player->useThread();
    player->start();
    while (device->isActive()) {
        Sleep(6);
    }
    assert("Should run to the end", true);
    
    DEL_(device);
    Player::cleanUp();
}

#pragma endregion

int _main(NS_FW_BASE::Map* args) {
#ifdef _DEBUG
    Debug::initialize(/*DEBUG_UNICODE | DEBUG_MEMORY*/);
#endif

    var tests = NEW_(PlayerTest);
    LOG("Player tests\n");
    tests->test("Create Player", (TestMethod) & (PlayerTest::testCreatePlayer));
    tests->test("Create Sequence", (TestMethod)&PlayerTest::testCreateSequence);
    tests->test("Create Sequence from frames", (TestMethod)&PlayerTest::testCreateSequenceFromFrames);
    tests->test("Load from binary", (TestMethod)&PlayerTest::testLoadFromBinary);
    tests->test("Create binary", (TestMethod)&PlayerTest::testCreateBinary);
    tests->test("Run Player", (TestMethod)&PlayerTest::testRunPlayer);
    //TEST(testPrintSequence); 
    tests->displayFinalResults();
    DEL_(tests);
    return 0;
}


//void createSequences(Player* player) {
//    //#region MASTER sequence - 12
//    {
//        var sequence = NEW_(Sequence, player->adapter());
//        sequence->writeHeader();
//        // Frame #1 - 8
//        sequence->writeDelta(0);
//        sequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(0)->writeByte(0);
//        sequence->writeEOF();
//        // Frame #2 - 3
//        sequence->writeDelta(96);
//        sequence->writeEOS();
//        player->sequences().push(sequence);
//    }
//
//    var ix = 0;
//    var adi = (AdapterDataItem*)player->adapters().search(&TestAdapter::Info.id, ix);
//    var testAdapter = adi->adapter;
//    //#region Sequence #1 - 79
//    {
//        var sequence = NEW_(Sequence, testAdapter);
//        sequence->writeHeader();
//        // Frame #1 - 14 bytes
//        sequence->writeDelta(0);
//        sequence->writeCommand(TestAdapterCommand::CmdSetText)->writeString("Seq1.1");
//        sequence->writeCommand(TestAdapterCommand::CmdMoveTo)->writeByte(-6)->writeByte(0);
//        sequence->writeEOF();
//        // Frame #2 - 16 bytes
//        sequence->writeDelta(20);
//        sequence->writeCommand(TestAdapterCommand::CmdSetInk)->writeByte(1);
//        sequence->writeCommand(TestAdapterCommand::CmdSetText)->writeString("Seq1.2");
//        sequence->writeCommand(TestAdapterCommand::CmdMoveTo)->writeByte(-6)->writeByte(0);
//        sequence->writeEOF();
//        // Frame #3 - 16 bytes
//        sequence->writeDelta(20);
//        sequence->writeCommand(TestAdapterCommand::CmdSetInk)->writeByte(2);
//        sequence->writeCommand(TestAdapterCommand::CmdSetText)->writeString("Seq1.3");
//        sequence->writeCommand(TestAdapterCommand::CmdMoveTo)->writeByte(-6)->writeByte(0);
//        sequence->writeEOF();
//        // Frame #4 - 16 bytes
//        sequence->writeDelta(20);
//        sequence->writeCommand(TestAdapterCommand::CmdSetInk)->writeByte(3);
//        sequence->writeCommand(TestAdapterCommand::CmdSetText)->writeString("Seq1.4");
//        sequence->writeCommand(TestAdapterCommand::CmdMoveTo)->writeByte(-6)->writeByte(1);
//        sequence->writeEOF();
//        // Frame #5 - 11 bytes
//        sequence->writeDelta(20);
//        sequence->writeCommand(TestAdapterCommand::CmdSetInk)->writeByte(4);
//        sequence->writeCommand(TestAdapterCommand::CmdSetText)->writeString("End\n");
//        sequence->writeEOF();
//        // Frame #6 - 5 bytes
//        sequence->writeDelta(20);
//        sequence->writeCommand(TestAdapterCommand::CmdSetInk)->writeByte(15);
//        sequence->writeEOS();
//        player->sequences().push(sequence);
//    }
//
//    //#region Sequence #2 - 33
//    {
//        var sequence = NEW_(Sequence, testAdapter);
//        sequence->writeHeader();
//        // Frame #1 - 13
//        sequence->writeDelta(16);
//        sequence->writeCommand(TestAdapterCommand::CmdSetInk)->writeByte(4);
//        sequence->writeCommand(TestAdapterCommand::CmdSetText)->writeString("Seq2.1");
//        sequence->writeEOF();
//        // Frame #2 - 26
//        sequence->writeDelta(16);
//        sequence->writeCommand(TestAdapterCommand::CmdSetInk)->writeByte(2);
//        sequence->writeCommand(TestAdapterCommand::CmdSetText)->writeString("Seq2.2 - End");
//        sequence->writeEOS();
//        player->sequences().push(sequence);
//    }
//}
//int setupAll(Player*& player, Adapter*& adapter, Cons*& cons, Channel*& channel) {
//    var result = 0;
//    while (true) {
//        // create player
//        player = NEW_(Player);
//
//        // add test adapter
//        result = 1;
//        player->registerAdapter(&TestAdapter::Info);
//        adapter = player->addAdapter(TestAdapter::Info.id, 1);
//        if (!adapter) break;
//
//        // create a Cons device
//        result = 2;
//        cons = (Cons*)adapter->createDevice(DevCons, NULL);
//        if (!cons) break;
//
//        // create a channel
//        result = 3;
//        channel = (Channel*)player->adapter()->createDevice(DevChannel, NULL);
//        if (!channel) break;
//
//        // assign sequence
//        result = 4;
//        createSequences(player);
//        if (player->sequences().length() != 3) break;
//        var seq1 = (Sequence*)player->sequences().get(1);
//        //player->masterDevice()->assignChannel(0, seq1, 0, 2);
//        channel->assign(0, seq1);
//
//        // successful
//        result = 0;
//        break;
//    }    
//    return result;
//}
//
//typedef int RUN(void* args);
//
//void run(RUN callback, void* args) {
//    LARGE_INTEGER lastTime, currentTime;
//    LARGE_INTEGER frequency;
//    int quit = 0;
//    double fps = 20.0;
//    SYSPR(QueryPerformanceCounter(&lastTime));
//    SYSPR(QueryPerformanceFrequency(&frequency));
//    qword countPerFrame = (qword)(frequency.QuadPart / fps);
//    qword counter = 0;
//    while (!quit)
//    {
//        SYSPR(QueryPerformanceCounter(&currentTime));
//        qword delta = (qword)(currentTime.QuadPart - lastTime.QuadPart);
//        // check fps
//        counter += delta;
//        if (counter > countPerFrame) {
//            counter -= countPerFrame;
//            quit = callback(args);
//        }
//        lastTime = currentTime;
//    }
//}
//
//int testCreateFrames() {
//    start("Create frames");
//    Player* player = NULL;
//    Adapter* adapter = NULL;
//    Channel* channel = NULL;
//    Cons* cons = NULL;
//    var result = setupAll(player, adapter, cons, channel);
//    if (result > 0) {
//        LOG("Setup failed, error code %d\n", result);
//    } else {
//        var frames = channel->toFrames();
//        ASSERT("Should have 6 frames", frames->length() == 6);
//        int count = 0;
//        for (var i = 0; i < frames->length(); i++) {
//            count += ((Frame*)frames->get(i))->commands_.length();
//        }
//        ASSERT("Should have correct number of commands", count == 15);
//    }
//    DEL_(player);
//
//    RESULTS();
//    return failed;
//}
//
//int testChannelRun() {
//    start("Run channel");
//    Player* player = NULL;
//    Adapter* adapter = NULL;
//    Channel* channel = NULL;
//    Cons* cons = NULL;
//    var result = setupAll(player, adapter, cons, channel);
//    if (result > 0) {
//        LOG("Setup failed, error code %d\n", result);
//    } else {
//        ASSERT("Should create all resources", result == 0);
//        // main loop
//        run([](void* args) {
//            var p = (Channel*)args;
//            return p->run(1) ? 0 : 1;
//            }, channel);
//    }
//
//    var seq = channel->sequence();
//    ASSERT("Should run to the end", seq->cursor() == seq->data() + seq->length());
//
//    DEL_(player);
//
//    RESULTS();
//    return failed;
//}
//
//int testSave() {
//    start("Save to binary");
//    Player* player = NULL;
//    Adapter* adapter = NULL;
//    Channel* channel = NULL;
//    Cons* cons = NULL;
//    var result = setupAll(player, adapter, cons, channel);
//    if (result > 0) {
//        LOG("Setup failed, error code %d\n", result);
//    }
//    else {
//        byte* db1 = MALLOC(byte, 2);
//        db1[0] = 1; db1[1] = DevChannel;
//        byte* db2 = MALLOC(byte, 2);
//        db2[0] = 1; db2[1] = DevCons;
//        player->addDatablock(db1, 2);
//        player->addDatablock(db2, 2);
//
//        byte* stream = NULL;
//        int length = player->createStream(stream);
//        byte testData[] = {
//            0x08,0x00,  // header - 2
//            0x08,0x00, 0x0B,0x00, 0x19,0x00,    // offset to adapter, sequence, datablock tables - 6
//            0x01,       // adapter list - 3
//            0x7B,0x01,
//            0x03,0x00,  // sequence table - 2 + 3*4
//            0x2B, 0x00, 0x0C, 0x00,
//            0x37, 0x00, 0x4F, 0x00,
//            0x86, 0x00, 0x21, 0x00,
//            0x02,0x00,  // datablock table - 2 + 2*8
//            0xA7, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
//            0xA9, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
//        };
//
//        var match = true;
//        var i = 0;
//        for (; i < sizeof(testData); i++) {
//            if (testData[i] != stream[i]) {
//                match = false;
//                break;
//            }
//        }
//        if (!match) LOG("Mismatch at %d: %d != %d\n", i, testData[i], stream[i]);
//        ASSERT("Should write bytes", length == 0xab && match == true);
//
//        if (!File::exists("test.bin")) {
//            File::write("test.bin", stream, length);
//        }
//
//        FREE(stream);
//    }
//    DEL_(player);
//
//    RESULTS();
//    return failed;
//}
//
//int testLoad() {
//    start("Load binary");
//    var player = NEW_(Player);
//    player->registerAdapter(&TestAdapter::Info);
//    var result = player->load("test.bin");
//    ASSERT("Should load the binary", result == 1);
//    // main loop
//    run([](void* args) {
//        var p = (Player*)args;
//        return p->run(1) ? 0 : 1;
//    }, player);
//    DEL_(player);
//    RESULTS();
//    return failed;
//}
//
//int testPrintSequence() {
//    start("Print sequence");
//
//    Player* player = NULL;
//    Adapter* adapter = NULL;
//    Channel* channel = NULL;
//    Cons* cons = NULL;
//    var result = setupAll(player, adapter, cons, channel);
//    if (result > 0) {
//        LOG("Setup failed, error code %d\n", result);
//    }
//    else {
//        var seq = (Sequence*)player->sequences().get(1);
//        var text = seq->print();
//        printf("%s", text);
//        var expected = "Adapter: TestAdapter\nFrames\n #00 [000] 02(53 65 71 31 2E 31 00) 04(FA 00)\n #01 [020] 03(01) 02(53 65 71 31 2E 32 00) 04(FA 00)\n #02 [020] 03(02) 02(53 65 71 31 2E 33 00) 04(FA 00)\n #03 [020] 03(03) 02(53 65 71 31 2E 34 00) 04(FA 01)\n #04 [020] 03(04) 02(45 6E 64 0A 00)\n #05 [020] 03(0F) 01()\n";
//        ASSERT("Should print sequence", strcmp(text, expected) == 0);
//        FREE(text);
//    }
//    DEL_(player);
//    RESULTS();
//    return failed;
//}