#include "consoleapp.h"
#include "player.h"
#include "test-adapter.h"
#include "test.h"

NS_FW_BASE_USE
using namespace PLAYER;

int testCreatePlayer();
int testCreateSequence();
int testCreateChannel();
int testChannelRun();
int testSave();
int testLoad();
//int testPlayerRun();

void createSequences(Player * player) {
    //#region MASTER sequence - 12
    {
        var sequence = NEW_(Sequence, player);
        sequence->writeHeader();
        // Frame #1 - 8
        sequence->writeDelta(0);
        sequence->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(0)->writeByte(0);
        sequence->writeEOF();
        // Frame #2 - 3
        sequence->writeDelta(96);
        sequence->writeEOS();
        player->sequences().add(sequence);
    }

    var ix = 0;
    var adi = (AdapterDataItem*)player->adapters().search(&TestAdapter::Info.id, ix);
    var testAdapter = adi->adapter;
    //#region Sequence #1 - 79
    {
        var sequence = NEW_(Sequence, testAdapter);
        sequence->writeHeader();
        // Frame #1 - 14 bytes
        sequence->writeDelta(0);
        sequence->writeCommand(TestAdapterCommand::CmdSetText)->writeString("Seq1.1");
        sequence->writeCommand(TestAdapterCommand::CmdMoveTo)->writeByte(-6)->writeByte(0);
        sequence->writeEOF();
        // Frame #2 - 16 bytes
        sequence->writeDelta(20);
        sequence->writeCommand(TestAdapterCommand::CmdSetInk)->writeByte(1);
        sequence->writeCommand(TestAdapterCommand::CmdSetText)->writeString("Seq1.2");
        sequence->writeCommand(TestAdapterCommand::CmdMoveTo)->writeByte(-6)->writeByte(0);
        sequence->writeEOF();
        // Frame #3 - 16 bytes
        sequence->writeDelta(20);
        sequence->writeCommand(TestAdapterCommand::CmdSetInk)->writeByte(2);
        sequence->writeCommand(TestAdapterCommand::CmdSetText)->writeString("Seq1.3");
        sequence->writeCommand(TestAdapterCommand::CmdMoveTo)->writeByte(-6)->writeByte(0);
        sequence->writeEOF();
        // Frame #4 - 16 bytes
        sequence->writeDelta(20);
        sequence->writeCommand(TestAdapterCommand::CmdSetInk)->writeByte(3);
        sequence->writeCommand(TestAdapterCommand::CmdSetText)->writeString("Seq1.4");
        sequence->writeCommand(TestAdapterCommand::CmdMoveTo)->writeByte(-6)->writeByte(1);
        sequence->writeEOF();
        // Frame #5 - 11 bytes
        sequence->writeDelta(20);
        sequence->writeCommand(TestAdapterCommand::CmdSetInk)->writeByte(4);
        sequence->writeCommand(TestAdapterCommand::CmdSetText)->writeString("End\n");
        sequence->writeEOF();
        // Frame #6 - 5 bytes
        sequence->writeDelta(20);
        sequence->writeCommand(TestAdapterCommand::CmdSetInk)->writeByte(15);
        sequence->writeEOS();
        player->sequences().add(sequence);
    }

    //#region Sequence #2 - 33
    {
        var sequence = NEW_(Sequence, testAdapter);
        sequence->writeHeader();
        // Frame #1 - 13
        sequence->writeDelta(16);
        sequence->writeCommand(TestAdapterCommand::CmdSetInk)->writeByte(4);
        sequence->writeCommand(TestAdapterCommand::CmdSetText)->writeString("Seq2.1");
        sequence->writeEOF();
        // Frame #2 - 26
        sequence->writeDelta(16);
        sequence->writeCommand(TestAdapterCommand::CmdSetInk)->writeByte(2);
        sequence->writeCommand(TestAdapterCommand::CmdSetText)->writeString("Seq2.2 - End");
        sequence->writeEOS();
        player->sequences().add(sequence);
    }
}

int setupAll(Player*& player, Adapter*& adapter, Cons*& cons, Channel*& channel) {
    var result = 0;
    while (true) {
        // create player
        result++;
        Player::registerAdapter(TestAdapter::Info);
        player = (Player*)Player::creator(NULL);
        if (!player) break;

        // add test adapter
        result++;
        adapter = player->addAdapter(TestAdapter::Info.id, 1);
        if (!adapter) break;

        // create a Cons device
        result++;
        adapter->createDevice(DevCons, NULL);
        cons = (Cons*)adapter->devices().getAt(0);
        if (!cons) break;

        // create a channel
        result++;
        channel = (Channel*)player->createDevice(DevChannel, NULL);
        channel->adapter(adapter);
        if (!channel) break;

        // assign sequence
        result++;
        createSequences(player);
        if (player->sequences().length() != 3) break;
        var seq1 = (Sequence*)player->sequences().getAt(1);
        channel->assign(0, seq1);

        // successful
        result = 0;
        break;
    }    
    return result;
}

typedef int RUN(void* args);

void run(RUN callback, void* args) {
    LARGE_INTEGER lastTime, currentTime;
    LARGE_INTEGER frequency;
    int quit = 0;
    double fps = 20.0;
    SYSPR(QueryPerformanceCounter(&lastTime));
    SYSPR(QueryPerformanceFrequency(&frequency));
    qword countPerFrame = (qword)(frequency.QuadPart / fps);
    qword counter = 0;
    while (!quit)
    {
        SYSPR(QueryPerformanceCounter(&currentTime));
        qword delta = (qword)(currentTime.QuadPart - lastTime.QuadPart);
        // check fps
        counter += delta;
        if (counter > countPerFrame) {
            counter -= countPerFrame;
            quit = callback(args);
        }
        lastTime = currentTime;
    }
}

int _main(NS_FW_BASE::Map* args) {
    #ifdef _DEBUG
    Debug::initialize(/*DEBUG_UNICODE | DEBUG_MEMORY*/);
    #endif

    LOG("Player tests\n");
    int passed = 0, failed = 0;

    TEST(testCreatePlayer);
    TEST(testCreateSequence);
    TEST(testCreateChannel);
    TEST(testSave);
    TEST(testLoad);
    
    printf("*** Final results: %d/%d=%.02f%%\n\n", passed, (passed + failed), (100.0f*passed) / (passed + failed));
    return 0;
}

int testCreatePlayer() {
    HEADER("Create player");

    var player = (Player*)Player::creator(NULL);

    ASSERT("Should register the Player adapter", Player::adapterTypes().containsKey(&player->info()->id) == true);
    ASSERT("Should add 1 adapter to the player", player->adapters().length() == 1);
    ASSERT("Should have 2 devices", player->devices().length() == 2);
    ASSERT("Should have player as device #0", player->devices().getAt(0) == player);
    ASSERT("Should have the master channel as device #1", player->devices().getAt(1) == player->masterChannel());

    DEL_(player);

    RESULTS();
    return failed;
}

int testCreateSequence() {
    HEADER("Create sequence");

    // register TestAdapter
    Player::registerAdapter(TestAdapter::Info);
    var player = (Player*)Player::creator(NULL);
    player->addAdapter(TestAdapter::Info.id, 0);
    createSequences(player);

    var sequence = (Sequence*)player->sequences().getAt(0);
    ASSERT("Should have 4 sequences", player->sequences().length() == 3);
    ASSERT("Master sequence should be 12 bytes long", sequence->length() == 12);
    sequence = (Sequence*)player->sequences().getAt(1);
    ASSERT("Sequence #1 should be 79 bytes long", sequence->length() == 79);
    sequence = (Sequence*)player->sequences().getAt(2);
    ASSERT("Sequence #2 should be 33 bytes long", sequence->length() == 33);

    DEL_(player);

    RESULTS();
    return failed;
}

int testCreateChannel() {
    HEADER("Create channel");
    Player* player = NULL;
    Adapter* testAdapter = NULL;
    Cons* cons = NULL;
    Channel* channel = NULL;
    var result = setupAll(player, testAdapter, cons, channel);
    if (result > 0) {
        LOG("Setup failed, error code %d\n", result);
    } else {
        ASSERT("Should create a channel", channel != NULL);
        var seq1 = (Sequence*)player->sequences().getAt(1);
        ASSERT("Should assign sequence #1", channel->sequence() == seq1);
        ASSERT("Should reset channel", channel->cursor() == seq1->data() + 15);
    }
    DEL_(player);

    RESULTS();
    return failed;
}

int testChannelRun() {
    HEADER("Run channel");
    Player* player = NULL;
    Adapter* testAdapter = NULL;
    Cons* cons = NULL;
    Channel* channel = NULL;
    var result = setupAll(player, testAdapter, cons, channel);
    if (result > 0) {
        LOG("Setup failed, error code %d\n", result);
    } else {
        ASSERT("Should create all resources", result == 0);
        // main loop
        run([](void* args) {
            var p = (Channel*)args;
            return p->run(1) ? 0 : 1;
            }, channel);
    }

    var seq = channel->sequence();
    ASSERT("Should run to the end", seq->cursor() == seq->data() + seq->length());

    DEL_(player);

    RESULTS();
    return failed;
}

int testSave() {
    HEADER("Save to binary");

    Player::registerAdapter(TestAdapter::Info);
    var player = (Player*)Player::creator(NULL);
    var adapter = player->addAdapter(TestAdapter::Info.id, 1);
    createSequences(player);
    byte* db1 = MALLOC(byte, 2);
    db1[0] = 1; db1[1] = DevChannel;
    byte* db2 = MALLOC(byte, 2);
    db2[0] = 1; db2[1] = DevCons;
    player->addDatablock(db1, 2);
    player->addDatablock(db2, 2);

    byte* stream = NULL;
    int length = player->createStream(stream);
    byte testData[] = {
        0x08,0x00,  // header - 2
        0x08,0x00, 0x0B,0x00, 0x19,0x00,    // offset to adapter, sequence, datablock tables - 6
        0x01,       // adapter list - 3
        0x7B,0x01,
        0x03,0x00,  // sequence table - 2 + 3*4
        0x2B, 0x00, 0x0C, 0x00,
        0x37, 0x00, 0x4F, 0x00,
        0x86, 0x00, 0x21, 0x00,
        0x02,0x00,  // datablock table - 2 + 2*8
        0xA7, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
        0xA9, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
    };

    var match = true;
    var i = 0;
    for (; i < sizeof(testData); i++) {
        if (testData[i] != stream[i]) {
            match = false;
            break;
        }
    }
    if (!match) LOG("Mismatch at %d: %d != %d\n", i, testData[i], stream[i]);
    ASSERT("Should write bytes", length == 0xab && match == true);

    if (!File::exists("test.bin")) {
        File::write("test.bin", stream, length);
    }

    FREE(stream);
    DEL_(player);

    RESULTS();
    return failed;
}

int testLoad() {
    HEADER("Load binary");

    Player::registerAdapter(TestAdapter::Info);
    var player = (Player*)Player::creator(NULL);
    var result = player->load("test.bin");
    ASSERT("Should load the binary", result == 1);
    // main loop
    run([](void* args) {
        var p = (Player*)args;
        return p->run(1) ? 0 : 1;
        }, player);

    DEL_(player);
    RESULTS();
    return failed;
}