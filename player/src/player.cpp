#include <stdio.h>
#include <windows.h>
#include "base/memory.h"
#include "utils/file.h"
#include "base/debug.h"
#include "player/src/player.h"
#include "player/src/sequence.h"
#include "player/src/channel.h"

#include "player-adapter.h"


NS_FW_BASE_USE
namespace PLAYER {

    Map* Player::adapters_ = NULL;
    Adapter* Player::addAdapter(Adapter* adapter) {
        if (Player::adapters_ == NULL) {
            Player::adapters_ = NEW_(Map, sizeof(int), MAP_USE_REF, Map::hashingInt, Collection::compareInt);
            //Player::adapters_->hasRefKey(false);
        }
        if (!Player::adapters_->containsKey(adapter->getInfo()->id)) {
            adapter->prepare();
            Player::adapters_->put(adapter->getInfo()->id, adapter);
        }
        
        return adapter;
    }
    Adapter* Player::getAdapter(int type) {
        Adapter* ad = NULL;
        if (Player::adapters_ != NULL && Player::adapters_->size() > 0) {
            ad = (Adapter*)Player::adapters_->get(type);
        }
        return ad;
    }

    void Player::cleanUp() {
        for (var i = 0; i < Player::adapters_->values()->length(); i++) {
            var adapter = (Adapter*)Player::adapters_->values()->get(i);
            adapter->cleanUp();
            DEL_(adapter);
        }
        DEL_(Player::adapters_);
        Player::adapters_ = NULL;
    }

    Player::Player() {
        threadId_ = 0;
        hThread_ = NULL;
        isTerminating_ = false;
        isPlaying_ = false;
        counter_ = 0;

        channels_.init(16);
        channels_.compare([](COLLECTION_ARGUMENTS) { return fmw::strncmp(((Channel*)value)->id(), (char*)key.p, 16); });
        devices_.init(16);
        devices_.compare([](COLLECTION_ARGUMENTS) { return ((Device*)value)->type() - key.i; });
        sequences_.init(16);
        sequences_.compare(Collection::compareByRef);
        frameLists_.init(16);
        dataBlocks_.init(sizeof(DataBlockItem), 16);
        dataBlocks_.compare(Collection::compareByRef);
        masterChannel_ = NULL;
        masterDevice_ = NULL;
        refreshRate_ = 25.0f;
        initData_.dataBlock = NULL;
    }
    Player::~Player() {
        clear();
        CloseHandle(hThread_);
    }

    //Sequence* Player::createSequence(byte* stream, int length) {
    //    var p = stream;
    //    var adapterType = READ(p, byte);
    //    int ix = -1;
    //    Adapter* adapter = NULL;
    //    for (var i = 0; i < adapters_.length(); i++) {
    //        var ai = (AdapterDataItem*)adapters_.get(i);
    //        if (ai->adapter->info()->id == adapterType) {
    //            adapter = ai->adapter;
    //            break;
    //        }
    //    }
    //    var sequence = NEW_(Sequence, adapter);
    //    sequence->writeBytes(stream, length);
    //    return sequence;
    //}
    
    #pragma region Threading
    void Player::useThread() {
        hThread_ = CreateThread(0, 0x1000, &Player::threadProc, this, CREATE_SUSPENDED, &threadId_);
        isPlaying_ = false;
        isTerminating_ = false;
    }

    DWORD Player::threadProc(void* lpParameter) {
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
        var player = (Player*)lpParameter;
        //var device = (PlayerDevice*)player->masterDevice();
        LARGE_INTEGER lastTime, currentTime;
        LARGE_INTEGER frequency;
        QueryPerformanceCounter(&lastTime);
        QueryPerformanceFrequency(&frequency);
        double countPerFrame = frequency.QuadPart / (double)player->refreshRate();
        double counter = 0;
        while (!player->isTerminating_)
        {
            QueryPerformanceCounter(&currentTime);
            qword delta = (qword)(currentTime.QuadPart - lastTime.QuadPart);
            // check fps
            counter += delta;
            int ticks = 0;
            while (counter > countPerFrame) {
                counter -= countPerFrame;
                ticks++;
            }
            if (!player->run(ticks)) break;
            lastTime = currentTime;
            //Sleep(6);
        }

        player->isPlaying_ = false;
        return 0;
    }

    void Player::start() {
        if (hThread_ != NULL) {
            useThread();
        }
        isPlaying_ = true;
        if (masterChannel_->sequence() == NULL) {
            var masterSequence = (Sequence*)sequences_.get(0);
            masterChannel_->assign(masterDevice_, masterSequence, 0);
        }
        isTerminating_ = false;
        SYSPR(ResumeThread(hThread_));
    }

    void Player::stop() {
        // signal stop to audio thread
        isTerminating_ = true;
        // wait for thread to finish
        while (isPlaying_) {
            Sleep(10);
        }
        dword exitCode;
        SYSPR(GetExitCodeThread(hThread_, &exitCode));
    }
    #pragma endregion

    #pragma region Utility
    DataBlockItem* Player::addDataBlock(byte* stream, int length, DataBlockItemFlag flag) {
        if (dataBlocks_.length() == 0) {
            // ensure the first datablock is reserved
            DataBlockItem dbi(0, NULL, DataBlockItemFlag::None);
            dataBlocks_.push(&dbi);
        }
        DataBlockItem dbi(length, stream, flag);
        return (DataBlockItem*)dataBlocks_.push(&dbi);
    }
    Sequence* Player::addSequence(Sequence* sequence) {
        sequences_.push(sequence);
        return sequence;
    }
    Sequence* Player::addSequence(byte* stream, int length) {
        var sequence = NEW_(Sequence, stream, 0, length);
        addSequence(sequence);
        return sequence;
    }
    Device* Player::addDevice(Adapter* adapter, int deviceType, byte** pData) {
        Device* device = NULL;
        var adapterId = adapter->getInfo()->id;
        if (!Player::adapters_->containsKey(adapterId)) {
            Player::adapters_->put(adapterId, adapter);
        }
        device = adapter->createDevice(deviceType, this);
        if (device) {
            device->initialize(pData);
            devices_.push(device);
            //device->dataBlockItem(dataBlock);
        }
        return device;
    }
    //Device* PlayerDevice::getDevice(int type) {
    //	int ix = -1;
    //	return (Device*)devices_.search(&type, ix);
    //}
    #pragma endregion


    //int Player::load(const char* path) {
    //    byte* buffer = NULL;
    //    int bytesRead = (int)File::read(path, &buffer);
    //    var result = 0;
    //    if (bytesRead > 0) {
    //        result = load(buffer);
    //        FREE(buffer);
    //    }
    //    else {
    //        result = -1;
    //    }
    //    return result;
    //}

    void Player::clear() {
        if (hThread_ != NULL) {
            stop();
        }

        dataBlocks_.apply(
            [](COLLECTION_ARGUMENTS) {
                var dbi = (DataBlockItem*)value;
                if (dbi->flag & DataBlockItemFlag::Allocated) {
                    FREE(dbi->dataBlock);
                }
                return value;
            }
        );
        dataBlocks_.clear();
        sequences_.apply(
            [](COLLECTION_ARGUMENTS) {
                DEL_((Sequence*)value);
                return value;
            });
        sequences_.clear();
        // don't delete the master device yet
        var i = devices_.length();
        while (--i > 0) {
            var dev = (Device*)devices_.get(i);
            DEL_(dev);
            devices_.remove(i);
        }
        frameLists_.apply(
            [](COLLECTION_ARGUMENTS) {
                DEL_((FrameList*)value)
                    return value;
            });
        frameLists_.clear();
        channels_.apply(
            [](COLLECTION_ARGUMENTS) {
                DEL_((Channel*)value);
                return value;
            });
        channels_.clear();

        if (initData_.dataBlock != NULL && (initData_.flag & DataBlockItemFlag::Allocated) != 0) {
            FREE(initData_.dataBlock);
        }
    }

    void Player::initialize(byte** pData) {
        clear();
        masterChannel_ = NEW_(Channel, "master");
        channels_.push(masterChannel_);
        if (pData != NULL && *pData != NULL) {
            // HEADER
            // 00 02 data block count
            // 02 04 length of data block #1 (adapter list)
            // 06 04 length of data block #2 (sequence list)
            // 0A 04 length of data block #3 (1st true data block)
            // ADAPTER LIST
            // adapter count
            // 1st adapter type
            // initialization data
            // device count
            // 1st device type
            // initialization data
            // ...
            var offset = *pData;
            var header = offset;
            var dataBlockCount = READ(offset, word);
            var dataBlockTable = (dword*)offset;
            var len = dataBlockCount * sizeof(dword);
            offset += len;
            // the header data block is the only allocated one
            initData_.dataBlock = header;
            initData_.length = (int)len;
            initData_.flag = DataBlockItemFlag::Allocated;

            // read data blocks
            var initData = offset; offset += dataBlockTable[0];
            var adapterList = offset; offset += dataBlockTable[1];
            var sequenceList = offset; offset += dataBlockTable[2];
            for (var i = 3; i < dataBlockCount; i++) {
                var length = dataBlockTable[i];
                DataBlockItem dbi(length, offset, DataBlockItemFlag::None);
                dataBlocks_.push(&dbi);
                offset += length;
            }

            // read init data
            var p = initData;
            refreshRate_ = READ(p, float);
            var channelCount = READ(p, byte);
            for (var ci = 0; ci < channelCount; ci++) {
                channels_.push(NEW_(Channel));
            }

            // process adapter list
            p = adapterList;
            var adapterCount = READ(p, byte);
            for (var ai = 0; ai < adapterCount; ai++) {
                // get adapter
                int adapterId = READ(p, byte);
                var adapter = (Adapter*)Player::adapters_->get(adapterId);
                if (adapter) {
                    // initialize
                    adapter->initialize(&p);
                    // create devices
                    var deviceCount = READ(p, byte);
                    for (var di = 0; di < deviceCount; di++) {
                        // create and initialize device
                        var device = adapter->createDevice(*p++, this);
                        if (device != NULL) {
                            device->initialize(&p);
                            devices_.push(device);
                        }
                        else {
                            // error: unknown device!
                        }
                    }
                }
                else {
                    // error: unknown adapter type!
                    break;
                }
            }

            // process sequence list
            var sequenceCount = READ(sequenceList, byte);
            offset = sequenceList + sequenceCount * sizeof(word);
            sequences_.init(sequenceCount);
            for (var i = 0; i < sequenceCount; i++) {
                var length = READ(sequenceList, word);
                addSequence(offset, length);
                offset += length;
            }

            // assign 1st sequence to the 1st channel with the 1st device
            //assignChannel(0, (Sequence*)sequences_.get(0), 0, 0);
        }
    }
    void Player::masterDevice(PlayerDevice* device) {
        masterDevice_ = device;
        devices_.insert(0, device);
    }

    Channel* Player::addChannel(char* id) {
        var channel = NEW_(Channel, id);
        channels_.push(channel);
        return channel;
    }
    //void Player::assignChannel(int channelId, Sequence* sequence, int deviceId, int loopCount) {
    //    var chn = (Channel*)channels_.get(channelId);
    //    var dev = (Device*)devices_.get(deviceId);
    //    chn->assign(dev, sequence, loopCount);
    //}
    int Player::run(int ticks) {
        if (masterChannel_->isActive()) {
            counter_ += ticks;
            for (var i = 0; i < channels_.length(); i++) {
                var ch = (Channel*)channels_.get(i);
                ch->run(ticks);
            }
        }
        return masterChannel_->isActive();
    }
}
