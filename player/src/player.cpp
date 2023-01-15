#include <stdio.h>
#include <windows.h>
#include "base/memory.h"
#include "utils/file.h"
#include "player.h"
#include "sequence.h"
#include "channel.h"

#include "adapter.h"

//#include <libloaderapi.h>
//#include "processthreadsapi.h"
//#include <synchapi.h>
//#include <profileapi.h>


NS_FW_BASE_USE
namespace PLAYER {

    Map* Player::adapters_ = NULL;
    Adapter* Player::addAdapter(Adapter* adapter) {
        if (Player::adapters_ == NULL) {
            Player::adapters_ = NEW_(Map, sizeof(int), sizeof(Adapter*), Map::hashingStr, Collection::compareInt);
        }
        Player::adapters_->put(&adapter->getInfo()->id, &adapter);
        return adapter;
    }
    Adapter* Player::getAdapter(int type) {
        return *(Adapter**)Player::adapters_->get(&type);
    }

    void Player::cleanUp() {
        for (var i = 0; i < Player::adapters_->values()->length(); i++) {
            var adapter = *(Adapter**)Player::adapters_->values()->get(i);
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

        channels_.init(16);
        channels_.compare([](void* p, UINT32 ix, Collection* collection, void* args) { return fmw::strncmp(((Channel*)p)->id(), (char*)args, 16); });
        devices_.init(16);
        devices_.compare([](void* p, UINT32 ix, Collection* collection, void* args) { return ((Device*)p)->type() - *(int*)args; });
        sequences_.init(16);
        sequences_.compare(Collection::compareByRef);
        dataBlocks_.init(sizeof(DataBlockItem), 16);
        dataBlocks_.compare(Collection::compareByRef);
        masterChannel_ = NULL;
        masterDevice_ = NULL;
        refreshRate_ = 25.0f;
        initData_.dataBlock = NULL;
    }
    Player::~Player() {
        clear();
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
        hThread_ = CreateThread(0, 0x1000, &Player::threadProc, this, 0, &threadId_);
        isPlaying_ = true;
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
            player->isTerminating_ = !player->run(ticks);
            lastTime = currentTime;
            //Sleep(6);
        }

        player->isPlaying_ = false;
        return 0;
    }


    void Player::start() {
        isPlaying_ = true;
        isTerminating_ = false;
        ResumeThread(hThread_);
    }

    void Player::stop() {
        // signal stop to audio thread
        isTerminating_ = true;
        // wait for thread to finish
        while (isPlaying_) {
            Sleep(1);
        }
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
        Sequence* sequence = NULL;
        int deviceId = *stream;
        int ix = -1;
        var device = (Device*)devices_.get(deviceId);
        if (device != NULL) {
            sequence = NEW_(Sequence, device, stream, 0, length);
            sequences_.push(sequence);
        }
        else {
            // error: illegal device id!
        }
        return sequence;
    }
    Device* Player::addDevice(Adapter* adapter, int deviceType, byte** pData) {
        Device* device = NULL;
        var adapterId = adapter->getInfo()->id;
        if (!Player::adapters_->containsKey(&adapterId)) {
            Player::adapters_->put(&adapterId, adapter);
        }
        device = adapter->createDevice(deviceType);
        if (device) {
            device->player(this);
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

        dataBlocks_.apply([](void* p, UINT32 ix, Collection* arr, void* args) {
            var dbi = (DataBlockItem*)p;
            if (dbi->flag & DataBlockItemFlag::Allocated) {
                FREE(dbi->dataBlock);
            }
            return 1;
            });
        sequences_.apply([](void* sequence, UINT32 ix, Collection* arr, void* args) {
            DEL_((Sequence*)sequence);
            return 1;
            });
        // don't delete the master device yet
        for (var i = 1; i < devices_.length(); i++) {
            var dev = (Device*)devices_.get(i);
            DEL_(dev);
        }
        channels_.apply([](void* channel, UINT32 ix, Collection* arr, void* args) {
            DEL_((Channel*)channel);
            return 1;
            });

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
                var adapter = *(Adapter**)Player::adapters_->get(&adapterId);
                if (adapter) {
                    // initialize
                    adapter->initialize(&p);
                    // create devices
                    var deviceCount = READ(p, byte);
                    for (var di = 0; di < deviceCount; di++) {
                        // create and initialize device
                        var device = adapter->createDevice(*p++);
                        if (device != NULL) {
                            device->player(this);
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
    void Player::assignChannel(int channelId, Sequence* sequence, int deviceId, int loopCount) {
        var chn = (Channel*)channels_.get(channelId);
        var dev = (Device*)devices_.get(deviceId);
        chn->assign(dev, sequence, loopCount);
    }
    int Player::run(int ticks) {
        if (masterChannel_->isActive()) {
            for (var i = 0; i < channels_.length(); i++) {
                ((Channel*)channels_.get(i))->run(ticks);
            }
        }
        return masterChannel_->isActive();
    }
    void Player::load(byte** pData) {
        initialize(pData);
    }
    Stream* Player::save() {
        var stream = NEW_(Stream, 1024);
        var start = stream->cursor();

        // create map of adapter to device list
        Map adapterMap(sizeof(Adapter*), sizeof(PArray*), Map::hashingStr, Adapter::compare);
        var list = NEW_(PArray, 16);
        var adapter = masterDevice_->adapter();
        adapterMap.put(&adapter, &list);
        for (var i = 1; i < devices_.length(); i++) {
            var device = (Device*)devices_.get(i);
            adapter = device->adapter();
            if (!adapterMap.containsKey(&adapter)) {
                list = NEW_(PArray, 16);
                adapterMap.put(&adapter, &list);
            }
            else {
                list = *(PArray**)adapterMap.get(&adapter);
            }
            list->push(device);
        }

        Stream initData(1024);
        initData.writeFloat(refreshRate_);
        // master channel is implicit
        initData.writeByte(channels_.length() - 1);

        // write adapter count
        Stream adapterList(4096);
        adapterList.writeByte((byte)adapterMap.size());
        for (var i = 0; i < adapterMap.size(); i++) {
            var adapter = *(Adapter**)adapterMap.keys()->get(i);
            // write adapter data
            adapter->writeToStream(&adapterList);
            var list = *(PArray**)adapterMap.values()->get(i);
            adapterList.writeByte(list->length());
            for (var j = 0; j < list->length(); j++) {
                var device = (Device*)list->get(j);
                device->writeToStream(&adapterList);
            }
            DEL_(list);
        }

        // create sequence list
        var seqCount = sequences_.length();
        Stream seqList(4096);
        seqList.writeByte(seqCount);
        for (var i = 0; i < seqCount; i++) {
            var seq = (Sequence*)sequences_.get(i);
            seqList.writeWord((word)seq->length());
        }
        for (var i = 0; i < seqCount; i++) {
            var seq = (Sequence*)sequences_.get(i);
            seqList.writeStream(seq);
        }

        // write header block
        word count = dataBlocks_.length() + 3;
        stream->writeWord(count);
        //int offset = sizeof(count) + count * sizeof(dword);
        stream->writeDword(initData.length());
        //offset += initData.length();
        stream->writeDword(adapterList.length());
        //offset += adapterList.length();
        stream->writeDword(seqList.length());
        //offset += seqList.length();
        for (var i = 0; i < dataBlocks_.length(); i++) {
            var dbi = (DataBlockItem*)dataBlocks_.get(i);
            stream->writeDword(dbi->length);
            //offset += dbi->length;
        }

        // write data
        stream->writeStream(&initData);
        stream->writeStream(&adapterList);
        stream->writeStream(&seqList);
        for (var i = 0; i < dataBlocks_.length(); i++) {
            var dbi = (DataBlockItem*)dataBlocks_.get(i);
            stream->writeBytes(dbi->dataBlock, dbi->length);
        }
        return stream;
    }
}
