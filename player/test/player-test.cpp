#include <windows.h>
#include "base/memory.h"
#include "base/debug.h"
#include "console/consoleapp.h"
#include "player/test/player-test.h"


int PlayerTest::LAST_TICK = 60;

int PlayerTest::onSetValue(void* obj, Value value, void* args) {
    var input = (Input*)obj;
    var playerTest = (PlayerTest*)args;
    playerTest->value1_ = *input->value();
    playerTest->value2_ = value;
    switch (input->type) {
    case InputTypeB:
        input->value()->b = 2 * value.b; break;
    case InputTypeF8:
        input->value()->b = 2 * value.b;
        ((InputF8*)input)->pValue()->f = input->value()->b / 255.0f;  break;
    case InputTypeF:
        input->value()->f = 2 * value.f; break;
    }
    return 1;
}

int PlayerTest::onSetPreset(void* obj, int ix, void* args) {
    var dev = (Device*)obj;
    var test = (PlayerTest*)args;
    test->preset_ = ix;
    return 1;
}

int PlayerTest::onSetPresetBank(void* obj, PresetBank* presetBank, void* args) {
    var dev = (Device*)obj;
    var test = (PlayerTest*)args;
    test->presetBank_ = presetBank;
    return 1;
}

void PlayerTest::testInput() {
    Value value;
    Input input(&value);
    input.Input::setup(10, 110, 6);
    input.set(22);
    assert("Should set the correct values", input.min.b == 10 && input.max.b == 106 && input.step.b == 6 && input.value()->b == 22);
    assert("Should set the assigned value as well", value.b == 22);
    input.set(6);
    assert("Should set the minimum", input.value()->b == 10);
    input.dec(2);
    assert("Should not decrease below minimum", input.value()->b == 10);
    input.inc(4);
    assert("Should increase by 24", input.value()->b == 34);
    input.set(112);
    assert("Should set the maximum", input.value()->b == 106);
    input.inc(2);
    assert("Should not increase above maximum", input.value()->b == 106);
    input.dec(5);
    assert("Should decrease by 30", input.value()->b == 76);
    input.setFromNormalized(0.6f);
    assert("Should set from normalized value", input.value()->b == 64);
    input.set.add(&input, PlayerTest::onSetValue, this);
    input.set(44);
    assert("Should set value using custom setter", value1_.b == 40 && value2_.b == 44 && input.value()->b == 88);
    input.set.removeAt(1);
    var f = input.getNormalized();
    assert("Should return normalized value", f == (88.0f - 10) / (106 - 10));
    char str[16];
    input.getValueAsString(str, 16);
    assert("Should return value as string", fmw::strncmp(str, "88", 16) == 0);
    Stream* stream = NEW_(Stream, 64);
    byte* ps = stream->data();
    input.writeToStream(stream);
    assert("Should write into the stream", ps[0] == input.min.b && ps[1] == input.max.b && ps[2] == input.step.b && ps[3] == input.value()->b);
    assert("Should write exact count of bytes", stream->length() == 4);
    stream->reset();
    input.writeValueToStream(stream);
    assert("Should write value into the stream", ps[0] == input.value()->b);
    assert("Should write exact count of bytes", stream->length() == 1);
    stream->reset();
    stream->writeByte(20);
    stream->writeByte(100);
    stream->writeByte(10);
    stream->writeByte(32);
    input.readFromStream(ps);
    assert("Should initialize from stream", input.min.b == 20 && input.max.b == 100 && input.step.b == 10 && input.value()->b == 30);
    DEL_(stream);
}

void PlayerTest::testInputF() {
    Value value;
    InputF input(&value);
    input.setup(-10.0f, 10.0f, 0.25f);
    input.set(0.7f);
    assert("Should set the correct values", input.min.f == -10.0f && input.max.f == 10.0f && input.step.f == 0.25f && input.value()->f == 0.75f);
    assert("Should set the assigned value as well", value.f == 0.75f);
    input.set(-12.0f);
    assert("Should set the minimum", input.value()->f == -10.0f);
    input.dec(2);
    assert("Should not decrease below minimum", input.value()->f == -10.0f);
    input.inc(4);
    assert("Should increase by 1.0", input.value()->f == -9.0f);
    input.set(11.0f);
    assert("Should set the maximum", input.value()->f == 10.0f);
    input.inc(2);
    assert("Should not increase above maximum", input.value()->f == 10.0f);
    input.dec(8);
    assert("Should decrease by 2.0", input.value()->f == 8.0f);
    input.setFromNormalized(0.36f);
    assert("Should set from normalized value", input.value()->f == -2.75f);
    input.set.add(&input, PlayerTest::onSetValue, this);
    input.set(4.3f);
    assert("Should set value using custom setter", value1_.f == 4.25f && value2_.f == 4.3f && input.value()->f == 8.6f);
    input.set.removeAt(1);
    var f = input.getNormalized();
    assert("Should return normalized value", f == (8.6f - -10.0f) / (10.0f - -10.0f));
    char str[16];
    input.getValueAsString(str, 16);
    assert("Should return value as string", fmw::strncmp(str, "8.60", 16) == 0);
    Stream* stream = NEW_(Stream, 64);
    byte* ps = stream->data();
    input.writeToStream(stream);
    assert("Should write into the stream", ((float*)ps)[0] == input.min.f && ((float*)ps)[1] == input.max.f && ((float*)ps)[2] == input.step.f && ((float*)ps)[3] == input.value()->f);
    assert("Should write exact count of bytes", stream->length() == 16);
    stream->reset();
    input.writeValueToStream(stream);
    assert("Should write value into the stream", ((float*)ps)[0] == input.value()->f);
    assert("Should write exact count of bytes", stream->length() == 4);
    stream->reset();
    stream->writeFloat(0.1f);
    stream->writeFloat(0.2f);
    stream->writeFloat(0.01f);
    stream->writeFloat(0.131f);
    input.readFromStream(ps);
    assert("Should initialize from stream", input.min.f == 0.1f && input.max.f == (0.1f + 10.0f*0.01f) && input.step.f == 0.01f && input.value()->f == 0.13f);
    DEL_(stream);
}

void PlayerTest::testInputF8() {
    Value value;
    InputF8 input(&value);
    input.setup(2, 104, 5);
    input.set(14);
    assert("Should set the correct values", input.min.b == 2 && input.max.b == 102 && input.step.b == 5 && input.value()->b == 12 && input.pValue()->f == 12 / 255.0f);
    assert("Should set the assigned value as well", value.f == 12 / 255.0f);
    input.set(1);
    assert("Should set the minimum", input.value()->b == 2 && value.f == 2 / 255.0f);
    input.dec(2);
    assert("Should set the minimum", input.value()->b == 2 && value.f == 2 / 255.0f);
    input.inc(4);
    assert("Should increase by 20", input.value()->b == 22 && value.f == 22 / 255.0f);
    input.set(112);
    assert("Should set the maximum", input.value()->b == 102 && value.f == 102 / 255.0f);
    input.inc(2);
    assert("Should not increase above maximum", input.value()->b == 102 && value.f == 102 / 255.0f);
    input.dec(5);
    assert("Should decrease by 25", input.value()->b == 77 && value.f == 77 / 255.0f);
    input.setFromNormalized(0.6f);
    assert("Should set from normalized value", input.value()->b == 62 && value.f == 62 / 255.0f);
    input.set.add(&input, PlayerTest::onSetValue, this);
    input.set(44);
    assert("Should set value using custom setter", value1_.b == 42 && value2_.b == 44 && input.value()->b == 88 && value.f == 88 / 255.0f);
    input.set.removeAt(1);
    var f = input.getNormalized();
    assert("Should return normalized value", f == (88.0f - 2) / (102 - 2));
    char str[16];
    input.getValueAsString(str, 16);
    assert("Should return value as string", fmw::strncmp(str, "88", 16) == 0);
    Stream* stream = NEW_(Stream, 64);
    byte* ps = stream->data();
    input.writeToStream(stream);
    assert("Should write into the stream", ps[0] == input.min.b && ps[1] == input.max.b && ps[2] == input.step.b && ps[3] == input.value()->b);
    assert("Should write exact count of bytes", stream->length() == 4);
    stream->reset();
    input.writeValueToStream(stream);
    assert("Should write value into the stream", ps[0] == input.value()->b);
    assert("Should write exact count of bytes", stream->length() == 1);
    stream->reset();
    stream->writeByte(20);
    stream->writeByte(100);
    stream->writeByte(10);
    stream->writeByte(32);
    input.readFromStream(ps);
    assert("Should initialize from stream", input.min.b == 20 && input.max.b == 100 && input.step.b == 10 && input.value()->b == 30 && input.pValue()->f == 30 / 255.0f);
    DEL_(stream);
}

void PlayerTest::testCreatePlayer() {
    var device = PlayerDevice::create(NULL);
    var adapter = Player::getAdapter(playerAdapter_.getInfo()->id);
    assert("Should add Player-Adapter", adapter != NULL && adapter->getInfo()->id == playerAdapter_.getInfo()->id);
    assert("Should create a Player-Device", device != NULL);
    assert("Should create wrapped Player object", device->object() != NULL);
    var player = device->player();
    assert("Should have a master Channel", player->masterChannel() != NULL && fmw::strncmp(player->masterChannel()->id(), "master") == 0);
    DEL_(device);
    Player::cleanUp();
}

void PlayerTest::testCreatePlayerExt() {
    var playerDevice = PlayerDevice::create(NULL);
    var playerExt = NEW_(PlayerExt, playerDevice->player());
    var playerDeviceExt = playerExt->getDeviceExtension(playerDevice);
    assert("Should create a PlayerExt-Device", playerDeviceExt != NULL);
    DEL_(playerDeviceExt);
    DEL_(playerExt);
    DEL_(playerDevice);
    Player::cleanUp();
}

void PlayerTest::testCreateConsDevice() {
    var playerDevice = PlayerDevice::create(NULL);
    var player = (Player*)playerDevice->object();
    var consAdapter = (ConsAdapter*)Player::addAdapter(NEW_(ConsAdapter));
    byte initData[] = { 0, 1, ConsoleColors::green };
    var pData = &initData[0];
    var consDevice = player->addDevice(consAdapter, ConsDevices::DeviceCons, &pData);
    assert("Should create a ConsDevice", consDevice != NULL);
    assert("Should be initialized correctly", consDevice->getInput(0)->value()->b == 0 && consDevice->getInput(1)->value()->b == 1 && consDevice->getInput(2)->value()->b == ConsoleColors::green);
    DEL_(playerDevice);
    Player::cleanUp();
}

void PlayerTest::testCreateConsDeviceExt() {
    var playerDevice = PlayerDevice::create(NULL);
    var player = playerDevice->player();
    var playerExt = NEW_(PlayerExt, player);

    byte initData[] = { 0xff, 1, 0, ConsoleColors::cyan };
    var pData = &initData[0];

    var consAdapter = (ConsAdapter*)Player::addAdapter(NEW_(ConsAdapter));
    var consDevice = player->addDevice(consAdapter, ConsDevices::DeviceCons, &pData);
    var consDeviceExt = playerExt->getDeviceExtension(consDevice);

    assert("Should create a ConsDevice", consDeviceExt != NULL);
    assert("Should bind to device", consDevice == consDeviceExt->device());
    DEL_(consDeviceExt);
    DEL_(playerExt);
    DEL_(playerDevice);
    Player::cleanUp();
}

void PlayerTest::testCreateConsDeviceExtSetters() {
    var playerDevice = PlayerDevice::create(NULL);
    var player = playerDevice->player();
    var playerExt = NEW_(PlayerExt, player);

    var bank = NEW_(Stream);
    // count
    bank->writeByte(3);
    // 
    bank->writeString("Default........")->writeByte(0)->writeByte(0)->writeByte(ConsoleColors::gray);
    bank->writeString("Preset#1.......")->writeByte(1)->writeByte(1)->writeByte(ConsoleColors::green);
    bank->writeString("Preset#2.......")->writeByte(0)->writeByte(3)->writeByte(ConsoleColors::black);
    var pb = NEW_(PresetBank, 3 * sizeof(Value::b), bank->data());
    DEL_(bank);

    var consAdapter = (ConsAdapter*)Player::addAdapter(NEW_(ConsAdapter));
    var consDevice = player->addDevice(consAdapter, ConsDevices::DeviceCons);
    var consDeviceExt = playerExt->getDeviceExtension(consDevice);
    
    assert("Should not have a preset bank", consDevice->presetBank() == NULL);
    presetBank_ = NULL;
    consDevice->setPresetBank.add(consDevice, PlayerTest::onSetPresetBank, this);
    consDevice->setPresetBank(pb);
    assert("Should have a preset bank with setter called", consDevice->presetBank() == pb && presetBank_ == pb);

    assert("Should have the default (0) preset", consDevice->preset() == 0);
    preset_ = NULL;
    consDevice->setPreset.add(consDevice, PlayerTest::onSetPreset, this);
    consDevice->setPreset(1);
    assert("Should have a preset with setter called", consDevice->preset() == 1 && preset_ == 1);

    DEL_(pb);
    DEL_(consDeviceExt);
    DEL_(playerExt);
    DEL_(playerDevice);
    Player::cleanUp();
}

void PlayerTest::createSequences(Player* player) {
    // sequence #1
    var sequence1 = NEW_(Sequence);
    {
        sequence1->writeByte(0);    // #0 device: PlayerDevice
        // Frame #1 - 8
        sequence1->writeDelta(0);
        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(1)->writeByte(0);
        sequence1->writeEOF();
        // Frame #2 - 3
        sequence1->writeDelta(PlayerTest::LAST_TICK);
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

void PlayerTest::testCreateSequence() {
    var device = PlayerDevice::create(NULL);
    var player = device->player();
    createSequences(player);
    assert("Should have 1 sequence", player->sequences().length() == 2);
    assert("Should have 12 bytes", ((Sequence*)player->sequences().get(0))->length() == 1 + 2 + 5 + 1 + 2 + 1);
    DEL_(device);
    Player::cleanUp();
}

Stream* toCommandStream(FrameList* frames, DeviceExt* deviceExt) {
    var merge = NEW_(Stream);
    for (var fi = 0; fi < frames->length(); fi++) {
        var frame = (Frame*)frames->get(fi);
        for (var ci = 0; ci < frame->commands.length(); ci++) {
            var cmd = (byte*)frame->commands.get(ci);
            var len = deviceExt->getCommandSize(cmd);
            merge->writeBytes(cmd, len);
        }
    }
    return merge;
}

void PlayerTest::testCreateAndEditFrames() {
    var playerDevice = PlayerDevice::create(NULL);
    var player = playerDevice->player();
    var playerExt = NEW_(PlayerExt, player);

    var consAdapter = (ConsAdapter*)Player::addAdapter(NEW_(ConsAdapter));
    byte initData[] = { 0xff, 0, 0, gray };
    byte* pData = initData;
    var consDevice = player->addDevice(consAdapter, ConsDevices::DeviceCons, &pData);
    var consDeviceExt = playerExt->getDeviceExtension(consDevice);

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

    var frames = NEW_(FrameList, consDeviceExt);
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

    var json = frames->toJSON(consDeviceExt);
    log("Frames as JSON\n%s\n", json);
    FREE(json);

    assert("Should have 6 frames", frames->length() == 6);
    // merge all commands into 1 stream
    var merge = toCommandStream(frames, consDeviceExt);
    int ix = -1;
    assert("Should have correct commands", memcmp(merge->data(), commands, merge->length(), ix) == 1);
    DUMP(commands, sizeof(commands), 32);
    DUMP(merge->data(), merge->length(), 32);
    DEL_(merge);

    frames->removeCommand(0, &commands[0]);
    var frame = (Frame*)frames->get(0);
    assert("Should have 1 less commands in frame #1", frame->commands.length() == 3);
    merge = toCommandStream(frames, consDeviceExt);
    DUMP(merge->data(), merge->length(), 32);
    DEL_(merge);

    DEL_(frames);
    DEL_(playerExt);
    DEL_(consDeviceExt);
    DEL_(playerDevice);
    Player::cleanUp();
}

void PlayerTest::testCreateFramesFromSequence() {
    var playerDevice = PlayerDevice::create(NULL);
    var player = playerDevice->player();
    var playerExt = NEW_(PlayerExt, player);

    var consAdapter = (ConsAdapter*)Player::addAdapter(NEW_(ConsAdapter));
    var consDevice = player->addDevice(consAdapter, ConsDevices::DeviceCons);
    var consDeviceExt = playerExt->getDeviceExtension(consDevice);

    createSequences(player);
    var seq = (Sequence*)player->sequences().get(1);
    var frames = FrameList::fromSequence(seq, consDeviceExt);
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

    var json = frames->toJSON(consDeviceExt);
    log("Frames as JSON\n%s\n", json);
    FREE(json);
    DEL_(frames);
    DEL_(playerExt);
    DEL_(consDeviceExt);
    DEL_(playerDevice);
    Player::cleanUp();
}

void PlayerTest::testCreateSequenceFromFrames() {
    var playerDevice = PlayerDevice::create(NULL);
    var player = playerDevice->player();
    var playerExt = NEW_(PlayerExt, player);

    var consAdapter = (ConsAdapter*)Player::addAdapter(NEW_(ConsAdapter));
    var consDevice = player->addDevice(consAdapter, ConsDevices::DeviceCons);
    var consDeviceExt = playerExt->getDeviceExtension(consDevice);

    createSequences(player);
    var seq1 = (Sequence*)player->sequences().get(1);
    var frames = FrameList::fromSequence(seq1, consDeviceExt);

    var seq2 = frames->toSequence(consDeviceExt);
    seq2->data()[0] = ConsDevices::DeviceCons;
    int pos = -1;
    assert("Should match original sequence",
        seq1->length() == seq2->length() &&
        fmw::memcmp(seq1->data(), seq2->data(), seq1->length(), pos) != false &&
        pos == seq1->length());
    log("Original sequence\n");
    DUMP(seq1->data(), seq1->length(), 32);
    log("Converted sequence\n");
    DUMP(seq2->data(), seq2->length(), 32);

    DEL_(seq2);
    DEL_(frames);
    DEL_(playerExt);
    DEL_(consDeviceExt);
    DEL_(playerDevice);
    Player::cleanUp();
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
        adapterList->writeByte(playerAdapter_.getInfo()->id);
        // no init data
        // no devices
        adapterList->writeByte(0);
    }
    // 2nd adapter: ConsAdapter
    {
        adapterList->writeByte(consAdapter_.getInfo()->id);
        adapterList->writeDword(0x12345678);
        // 1 device
        adapterList->writeByte(1);
        adapterList->writeByte(ConsDevices::DeviceCons);
        adapterList->writeByte(2);                          // x
        adapterList->writeByte(1);                          // y
        adapterList->writeByte(ConsoleColors::cyan);        // ink
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

void PlayerTest::testCreateBinary() {
    PlayerDeviceExt::registerExtensionCreator();
    var bin = createBinaryData();
    var data = bin->data();
    Player::addAdapter(NEW_(ConsAdapter));
    var playerDevice = PlayerDevice::create(&data);
    var playerExt = NEW_(PlayerExt, playerDevice->player());
    var playerDeviceExt = playerExt->getDeviceExtension(playerDevice);
    var stream = playerExt->save();

    //var stream2 = NEW_(Stream, 1024);
    //playerDeviceExt->writeToStream(stream2);

    var isMatch = binaryCompare(stream->data(), stream->length(), data, bin->length());
    //for (var i = 0; i < stream->length(); i++) {
    //    var ca = stream->readByte();
    //    var cb = stream2->readByte();
    //    if (ca != cb) {
    //        cons->printf(" - %04X: %02X %02X\n", i, ca, cb);
    //        isMatch = false;
    //        break;
    //    }
    //}
    //assert("Should create the same binary", isMatch == true);
    //DEL_(stream2);

    //var cons = getConsole();
    //cons->printf("Expected\n");
    //cons->dump(bin->data(), bin->length(), 16);
    //cons->printf("---\nReceived\n");
    //cons->dump(stream->data(), stream->length(), 16);
    //// compare bin and stream;
    //assert("Should be same long", bin->length() == stream->length());
    //
    //isMatch = true;
    //stream->rewind();
    //bin->rewind();
    //cons->printf("Binary comparison\n");
    //for (var i = 0; i < bin->length(); i++) {
    //    var ca = bin->readByte();
    //    var cb = stream->readByte();
    //    if (ca != cb) {
    //        cons->printf(" - %04X: %02X %02X\n", i, ca, cb);
    //        isMatch = false;
    //        break;
    //    }
    //}
    assert("Should be binary identical", isMatch);
    bin->extract();
    DEL_(stream);
    DEL_(bin);
    DEL_(playerExt);
    DEL_(playerDevice);
    DEL_(playerDeviceExt);
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

void PlayerTest::testRunChannelFromSequence() {
    var consAdapter = Player::addAdapter(NEW_(ConsAdapter));
    var device = PlayerDevice::create(NULL);
    var player = device->player();
    byte initData[] = { 0xff, 0, 0, ConsoleColors::gray };
    byte* pData = initData;
    var consDevice = player->addDevice(consAdapter, DeviceCons, &pData);
    var ch = player->addChannel("Chn1");
    createSequences(player);
    ch->assign(consDevice, (Sequence*)player->sequences().get(1));

    while (ch->isActive()) {
        ch->run(1);
        Sleep(20);
    }

    assert("Should run to the end", ch->loopCount() == -1);

    DEL_(device);
    Player::cleanUp();
}

void PlayerTest::testRunChannelFromFrames() {
    var consAdapter = Player::addAdapter(NEW_(ConsAdapter));
    var device = PlayerDevice::create(NULL);
    var player = device->player();
    byte initData[] = { 0xff, 0, 0, gray };
    byte* pData = initData;
    var consDevice = player->addDevice(&consAdapter_, DeviceCons, (byte**)&pData);
    var ch = player->addChannel("Chn1");
    createSequences(player);
    var consDeviceExt = NEW_(ConsDeviceExt, consDevice);
    var seq = (Sequence*)device->player()->sequences().get(1);
    var frames = FrameList::fromSequence(seq, consDeviceExt);
    DEL_(consDeviceExt);
    ch->assign(consDevice, frames);

    while (ch->isActive()) {
        ch->run(1);
        Sleep(20);
    }

    assert("Should run to the end", ch->loopCount() == -1);

    DEL_(frames);
    DEL_(device);
    Player::cleanUp();
}

void PlayerTest::testRunPlayerOnFrames() {
    var bin = createBinaryData();
    var consAdapter = Player::addAdapter(NEW_(ConsAdapter));
    var data = bin->extract();
    DEL_(bin);
    var playerDevice = PlayerDevice::create(&data);
    var player = playerDevice->player();
    var playerDeviceExt = NEW_(PlayerDeviceExt, playerDevice);
    var consDevice = (ConsDevice*)consAdapter->createDevice(ConsDevices::DeviceCons, player);
    var consDeviceExt = NEW_(ConsDeviceExt, consDevice);
    for (var si = 0; si < player->sequences().length(); si++) {
        DeviceExt* deviceExt = consDeviceExt;
        if (si == 0) deviceExt = playerDeviceExt;
        var seq = (Sequence*)player->sequences().get(si);
        var frames = FrameList::fromSequence(seq, deviceExt);
        player->frameLists().push(frames);
    }
    playerDevice->mode(PlayerModeFrameList);
    player->useThread();
    player->start();
    while (playerDevice->isActive()) {
        Sleep(10);
    }
    assert("Should run to the end", ((Channel*)player->channels().get(0))->loopCount() == -1);
    DEL_(consDevice);
    DEL_(consDeviceExt);
    DEL_(playerDeviceExt);
    DEL_(playerDevice);
    Player::cleanUp();
}

void PlayerTest::testRunPlayerOnSequence() {
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
    PlayerDeviceExt::registerExtensionCreator();
    ConsDeviceExt::registerExtensionCreator();

    test("Check Input methods", (TestMethod)&PlayerTest::testInput);
    test("Check InputF methods", (TestMethod)&PlayerTest::testInputF);
    test("Check InputF8 methods", (TestMethod)&PlayerTest::testInputF8);
    test("Create Player", (TestMethod)&PlayerTest::testCreatePlayer);

    test("Create PlayerExt", (TestMethod)&PlayerTest::testCreatePlayerExt);

    test("Create Cons device", (TestMethod)&PlayerTest::testCreateConsDevice);
    test("Create Cons ext device", (TestMethod)&PlayerTest::testCreateConsDeviceExt);
    test("Check setters", (TestMethod)&PlayerTest::testCreateConsDeviceExtSetters);

    test("Create Sequence", (TestMethod)&PlayerTest::testCreateSequence);
    test("Create and edit FrameList", (TestMethod)&PlayerTest::testCreateAndEditFrames);
    test("Create FrameList from Sequence", (TestMethod)&PlayerTest::testCreateFramesFromSequence);
    test("Create Sequence from FrameList", (TestMethod)&PlayerTest::testCreateSequenceFromFrames);

    test("Save to binary", (TestMethod)&PlayerTest::testCreateBinary);
    test("Load from binary", (TestMethod)&PlayerTest::testLoadFromBinary);

    test("Run channel from sequence", (TestMethod)&PlayerTest::testRunChannelFromSequence);
    test("Run channel from frames", (TestMethod)&PlayerTest::testRunChannelFromFrames);
    test("Run Player on sequence", (TestMethod)&PlayerTest::testRunPlayerOnSequence);
    test("Run Player on frames", (TestMethod)&PlayerTest::testRunPlayerOnFrames);

    displayFinalResults();

    PlayerExt::cleanUp();

    //TEST(testPrintSequence); 
}
