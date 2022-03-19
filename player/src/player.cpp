#include "base/memory.h"
#include "utils/file.h"
#include "player.h"
#include <stdio.h>

NS_FW_BASE_USE
namespace PLAYER {

    AdapterInfo PlayerInfo_ = { "Player", 0, Player::creator, Player::initializer };

    int CompareAdapters(void* item, unsigned int ix, Collection* array, void* key) {
        var adi = (AdapterDataItem*)item;
        return adi->adapter->info()->id - *(int*)key;
    }

    int CompareAdapterInfos(void* item, unsigned int ix, Collection* array, void* key) {
        var ai = (AdapterInfo*)item;
        return ai->id - *(int*)key;
    }

    //int CompareAdapters(void* item, unsigned int ix, Collection* array, void* key) {
    //    var adapter = (Adapter*)item;
    //    return adapter->info()->id - (int)key;
    //}

    //int CompareAdapters(void* item, unsigned int ix, Collection* array, void* key) {
    //    var adapter = (Adapter*)item;
    //    return adapter->info()->id - (int)key;
    //}

    //int CompareAdapters(void* item, unsigned int ix, Collection* array, void* key) {
    //    var adapter = (Adapter*)item;
    //    return adapter->info()->id - (int)key;
    //}

    Player::Player() {
        player(this);
        adapters_.init(sizeof(AdapterDataItem), 8);
        adapters_.compare(CompareAdapters);
        sequences_.init(8);
        channels_.init(16);
        userDataBlocks_.init(sizeof(UserDataBlockItem), 16);
        masterChannel_ = NULL;
        refreshRate_ = 25;
    }
    Player::~Player() {
        devices_.removeAt(0);
        adapters_.forEach([](void* adi, UINT32 ix, Collection* arr, void* args) {
            var adapter = ((AdapterDataItem*)adi)->adapter;
            if (adapter != args) {
                DEL_(((AdapterDataItem*)adi)->adapter); return 1;
            }
            return 1;
        }, this);
        sequences_.forEach([](void* sequence, UINT32 ix, Collection* arr, void* args) { DEL_((Sequence*)sequence); return 1; });
        //channels_.forEach([](void* channel, UINT32 ix, Collection* arr, void* args) { DEL_((Channel*)channel); return 1; });
        userDataBlocks_.forEach([](void* udbi, UINT32 ix, Collection* arr, void* args) { FREE(((UserDataBlockItem*)udbi)->userDataBlock); return 1; });
    }
    void Player::initialize(void* args) {
        // set adapter #0
        AdapterDataItem adi = { this, 0 };
        adapters_.add(&adi);

        // set device #0
        devices_.add(this);

        // create master channel
        masterChannel_ = (Channel*)createDevice(PlayerDevices::DevChannel, NULL);
        masterChannel_->id("master");
        masterChannel_->loopCount(0);
        masterChannel_->isActive(true);
    }

    // Adapter implementation
    AdapterInfo* Player::info() { return &PlayerInfo_; };

    Device* Player::createDeviceImpl(int deviceType, byte** initData) {
        Device* device = NULL;
        switch (deviceType) {
        case PlayerDevices::DevPlayer:
            device = NEW_(Player);
            break;
        case PlayerDevices::DevChannel:
            char id[16];
            sprintf_s(id, 16, "Chn%03d", channels_.length());
            device = NEW_(Channel, id);
            channels_.add(device);
            break;
        }
        return device;
    }

    byte* Player::processCommand(Channel* channel, byte command) {
        var device = channel->device();
        var sequence = channel->sequence();
        var cursor = channel->cursor();
        switch (command) {
        case PlayerCommands::CmdAssign: // chnId, seqId, devId, loop-count
            byte channelId; channelId = READ(cursor, byte);
            byte sequenceId; sequenceId = READ(cursor, byte);
            byte deviceId; deviceId = READ(cursor, byte);
            Channel* targetChannel; targetChannel = (Channel*)channels_.getAt(channelId);
            byte count; count = READ(cursor, byte);
            targetChannel->loopCount(count);
            Sequence* targetSequence; targetSequence = (Sequence*)sequences_.getAt(sequenceId);
            targetChannel->assign(deviceId, targetSequence);
            break;
        case PlayerCommands::CmdTempo: // fps
            var fps = READ(cursor, float);
            setRefreshRate(fps);
            break;
        }
        channel->cursor(cursor);
        return cursor;
    }

    void Player::setRefreshRate(float fps) {
        for (int i = 1; i < adapters_.length(); i++) {
            ((AdapterDataItem*)adapters_.getAt(i))->adapter->setRefreshRate(fps);
        }
        refreshRate_ = fps;
    }

    // Player methods
    int Player::load(const char* path) {
        byte* buffer = NULL;
        var bytesRead = File::read(path, &buffer);
        var result = 0;
        if (bytesRead > 0) {
            result = load(buffer);
            FREE(buffer);
        }
        else {
            result = -1;
        }
        return result;
    }
    int Player::load(byte* stream) {
        int result = 1;
        byte* p = (byte*) &((PlayerBinaryHeader*)stream)->adapterTableOffset;
        // read header
        var adapterTable = stream + READ(p, word);
        var sequenceTable = stream + READ(p, word);
        var dataBlockTable = stream + READ(p, word);

        // get data blocks
        int count = READ(dataBlockTable, word);
        userDataBlocks_.init(sizeof(UserDataBlockItem), count);
        UserDataBlockItem dataBlock = { 0, NULL };
        for (var i = 0; i < count; i++) {
            dataBlock.userDataBlock = stream + READ(dataBlockTable, dword);
            dataBlock.length = READ(dataBlockTable, dword);
            userDataBlocks_.add(&dataBlock);
        }

        // add adapters, prepare context
        count = READ(adapterTable, byte);
        adapters_.init(sizeof(AdapterDataItem), count);
        AdapterDataItem adapterData = { this, 0 };
        adapters_.add(&adapterData);
        prepareContext(((UserDataBlockItem*)userDataBlocks_.getAt(0))->userDataBlock);
        for (var i = 0; i < count; i++) {
            var adapterId = (int)READ(adapterTable, byte);
            var dataBlockId = (int)READ(adapterTable, byte);
            // create and prepare adapter
            var adapterInfo = (AdapterInfo*)Player::adapterTypes_.get(&adapterId);
            var adapter = adapterInfo->creator(this);
            var dataBlock = ((UserDataBlockItem*)userDataBlocks_.getAt(dataBlockId))->userDataBlock;
            adapterData.adapter = adapter;
            adapterData.userDataBlock = dataBlockId;
            adapters_.add(&adapterData);
            adapter->prepareContext(dataBlock);
        }

        // create sequences
        count = READ(sequenceTable, word);
        sequences_.init(count);
        for (var i = 0; i < count; i++) {
            var start = stream + READ(sequenceTable, word);
            var length = READ(sequenceTable, word);
            var sequence = createSequence(start, length);
            sequences_.add(sequence);
        }

        // assign master channel
        masterChannel_->assign(0, (Sequence*)sequences_.getAt(0));
        return result;
    }

    int Player::createStream(byte*& stream, Array* adapters, PArray* sequences, Array* userDataBlocks) {
        stream = MALLOC(byte, 65536);
        if (adapters == NULL) adapters = &adapters_;
        if (sequences == NULL) sequences = &sequences_;
        if (userDataBlocks == NULL) userDataBlocks = &userDataBlocks_;

        // create header

        var p = stream;
        var offset = 2 + 3 * 2;
        WRITE(p, offset, word);                 // header size
        WRITE(p, offset, word);                 // offset of adapter list
        offset += 1 + (adapters->length() - 1) * 2;
        WRITE(p, offset, word);                 // offset of sequence table
        offset += 2 + sequences->length() * 4;
        WRITE(p, offset, word);                 // offset of data block table
        offset += 2 + userDataBlocks->length() * 8;

        // write adapter list
        WRITE(p, adapters->length() - 1, byte);                 // adapter count
        for (var i = 1; i < adapters->length(); i++) {          // skip first as it is ALWAYS the Player
            var adapterData = ((AdapterDataItem*)adapters->getAt(i));
            WRITE(p, adapterData->adapter->info()->id, byte);   // adapter type
            WRITE(p, adapterData->userDataBlock, byte);         // id of data block of adapter initialization
        }

        // write sequence table
        WRITE(p, sequences_.length(), word);        // sequence count
        for (var i = 0; i < sequences_.length(); i++) {
            var sequence = (Sequence*)sequences_.getAt(i);
            WRITE(p, offset, word);                 // sequence offset
            int length = sequence->length();
            offset += length;
            WRITE(p, length, word);                 // sequence length
        }

        // write data block table
        WRITE(p, userDataBlocks_.length(), word);        // data block count
        for (var i = 0; i < userDataBlocks_.length(); i++) {
            var dbi = (UserDataBlockItem*)userDataBlocks_.getAt(i);
            WRITE(p, offset, dword);                    // data block offset
            offset += dbi->length;
            WRITE(p, dbi->length, dword);               // data block length
        }

        // write sequences
        for (var i = 0; i < sequences_.length(); i++) {
            var sequence = (Sequence*)sequences_.getAt(i);
            var si = sequence->data();
            while (si != sequence->cursor()) {
                WRITE(p, *si++, byte);
            }
        }

        // write data blocks
        for (var i = 0; i < userDataBlocks_.length(); i++) {
            var dbi = (UserDataBlockItem*)userDataBlocks_.getAt(i);
            var si = dbi->userDataBlock;
            var end = si + dbi->length;
            while (si != end) {
                WRITE(p, *si++, byte);
            }
        }

        int length = (int)(p - stream);
        stream = REALLOC(stream, byte, length);
        return length;
    }

    size_t Player::save(const char* path) {
        byte* stream = NULL;
        var length = createStream(stream);
        var size = File::write(path, stream, length);
        FREE(stream);
        return size;
    }

    Adapter* Player::addAdapter(int adapterType, int dataBlockId) {
        Adapter* adapter = NULL;
        var value = Player::adapterTypes_.get(&adapterType);
        if (value) {
            var adapterInfo = (AdapterInfo*)value;
            adapter = adapterInfo->creator(this);
            AdapterDataItem adi = { adapter, dataBlockId };
            adapters_.add(&adi);
        }
        return adapter;
    }

    UserDataBlockItem* Player::addDatablock(byte* stream, int length) {
        UserDataBlockItem udbi(length, stream);
        return (UserDataBlockItem*)userDataBlocks_.add(&udbi);
    }

    Sequence* Player::createSequence(byte* stream, int length) {
        var p = stream;
        var adapterType = READ(p, byte);
        int ix = -1;
        Adapter* adapter = NULL;
        for (var i = 0; i < adapters_.length(); i++) {
            var ai = (AdapterDataItem*)adapters_.getAt(i);
            if (ai->adapter->info()->id == adapterType) {
                adapter = ai->adapter;
                break;
            }
        }
        var sequence = NEW_(Sequence, adapter);
        sequence->writeBytes(stream, length);
        return sequence;
    }

    bool Player::run(int ticks) {
        if (masterChannel_->isActive()) {
            for (var i = 0; i < channels_.length(); i++) {
                ((Channel*)channels_.getAt(i))->run(ticks);
            }
        }
        return masterChannel_->isActive();
    }

    void Player::reset() {
        masterChannel_->assign(0, (Sequence*)sequences_.getAt(0));
        masterChannel_->loopCount(0);
        masterChannel_->isActive(true);
    }

    bool Player::isActive() {
        return ((Channel*)channels_.getAt(0))->isActive();
    }

    byte* Player::makeCommand(byte command, Sequence* sequence, byte* cursor) {
        var stream = MALLOC(byte, 256);
        //var stream = new Stream(128);
        //if (typeof command == 'string') {
        //    command = Ps.Player.Commands[command.toUpperCase()];
        //}
        //stream.writeUint8(command);
        //var inputStream = null;
        //if (arguments[1] instanceof Ps.Sequence) inputStream = arguments[1].stream;
        //else if (arguments[1] instanceof Stream) inputStream = arguments[1];
        //switch (command) {
        //case Ps.Player.Commands.Assign:
        //    if (inputStream) {
        //        stream.writeStream(inputStream, arguments[2], 4);
        //    }
        //    else {
        //        stream.writeUint8(arguments[1]);    // channel id
        //        stream.writeUint8(arguments[2]);    // sequence id
        //        stream.writeUint8(arguments[3]);    // device id
        //        stream.writeUint8(arguments[4]);    // loop count
        //    }
        //    break;
        //case Ps.Player.Commands.Tempo:
        //    if (inputStream) {
        //        stream.writeStream(inputStream, arguments[2], 4);
        //    }
        //    else {
        //        stream.writeFloat32(arguments[1]);
        //    }
        //    break;
        //case Ps.Player.Commands.EOF:
        //    //stream.writeUint8(Ps.Player.Commands.EOF);
        //    break;
        //case Ps.Player.Commands.EOS:
        //    //stream.writeUint8(Ps.Player.Commands.EOS);
        //    break;
        //}

        //stream.buffer = stream.buffer.slice(0, stream.length);
        return stream;
    }

    // static members
    Map Player::adapterTypes_(sizeof(AdapterInfo::id), sizeof(AdapterInfo), Map::hashingItem, Collection::compareInt);
    Map& Player::adapterTypes() { return Player::adapterTypes_; }
    
    void Player::registerAdapter(AdapterInfo& info) {
        if (!Player::adapterTypes_.containsKey(&info.id)) {
            info.initializer(NULL);
            Player::adapterTypes_.put(&info.id, &info);
        }
    }

    AdapterInfo* Player::getAdapterInfoByName(const char* name) {
        AdapterInfo* adapterInfo = NULL;
        var values = Player::adapterTypes_.values();
        for (var i = 0; i < values->length(); i++) {
            var ai = (AdapterInfo*)values->getAt(i);
            if (strncmp(ai->name, name, 16) == 0) {
                adapterInfo = ai;
                break;
            }
        }
        return adapterInfo;
    }

    Adapter* Player::creator(Player* notUsed) {
        if (!Player::adapterTypes_.containsKey(&PlayerInfo_.id)) {
            Player::registerAdapter(PlayerInfo_);
        }
        var player = NEW_(Player);
        player->initialize(player);
        return player;
    }
    void Player::initializer(void* args) {
    }
}