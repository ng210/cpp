#include "base/memory.h"
#include "base/stream.h"
#include "player/src/device-ext.h"
#include "player/src/player-ext.h"

using namespace PLAYER;

Map PlayerExt::deviceExtensions_(sizeof(int), MAP_USE_REF, Map::hashingInt, Collection::compareInt);

//void PlayerExt::load(Player* player, byte** pData) {
//
//}

PlayerExt::PlayerExt(Player* player) {
    player_ = player;
}

Stream* PlayerExt::save() {
    var stream = NEW_(Stream, 1024);
    var start = stream->cursor();

    // create map of adapter to device list
    // - get all adapter types, start with master device
    //   - get all devices by adapter type
    Map adapterMap(MAP_USE_REF, MAP_USE_REF, Map::hashingStr, Adapter::compare);
    PArray* list = NULL;
    for (var i = 0; i < player_->devices().length(); i++) {
        var device = (Device*)player_->devices().get(i);
        var adapter = device->adapter();
        if (!adapterMap.containsKey(adapter)) {
            list = NEW_(PArray, 16);
            adapterMap.put(adapter, list);
        }
        else {
            list = (PArray*)adapterMap.get(adapter);
        }
        if (device != player()->masterDevice()) {
            list->push(device);
        }
    }

    Stream initData(1024);
    initData.writeFloat(player_->refreshRate());
    // master channel is implicit
    initData.writeByte(player_->channels().length() - 1);

    // write adapter count
    Stream adapterList(4096);
    adapterList.writeByte((byte)adapterMap.size());
    for (var i = 0; i < adapterMap.size(); i++) {
        var adapter = (Adapter*)adapterMap.keys()->get(i);
        // write adapter data
        adapter->writeToStream(&adapterList);
        var list = (PArray*)adapterMap.values()->get(i);
        adapterList.writeByte(list->length());
        for (var j = 0; j < list->length(); j++) {
            var deviceExt = getDeviceExtension((Device*)list->get(j));
            deviceExt->writeToStream(&adapterList);
            DEL_(deviceExt);
        }
        DEL_(list);
    }

    // create sequence list
    var seqCount = player_->sequences().length();
    Stream seqList(4096);
    seqList.writeByte(seqCount);
    for (var i = 0; i < seqCount; i++) {
        var seq = (Sequence*)player_->sequences().get(i);
        seqList.writeWord((word)seq->length());
    }
    for (var i = 0; i < seqCount; i++) {
        var seq = (Sequence*)player_->sequences().get(i);
        seqList.writeStream(seq);
    }

    // write header block
    word count = player_->dataBlocks().length() + 3;
    stream->writeWord(count);
    //int offset = sizeof(count) + count * sizeof(dword);
    stream->writeDword(initData.length());
    //offset += initData.length();
    stream->writeDword(adapterList.length());
    //offset += adapterList.length();
    stream->writeDword(seqList.length());
    //offset += seqList.length();
    for (var i = 0; i < player_->dataBlocks().length(); i++) {
        var dbi = (DataBlockItem*)player_->dataBlocks().get(i);
        stream->writeDword(dbi->length);
        //offset += dbi->length;
    }

    // write data
    stream->writeStream(&initData);
    stream->writeStream(&adapterList);
    stream->writeStream(&seqList);
    for (var i = 0; i < player_->dataBlocks().length(); i++) {
        var dbi = (DataBlockItem*)player_->dataBlocks().get(i);
        stream->writeBytes(dbi->dataBlock, dbi->length);
    }
    return stream;
}

DeviceExt* PlayerExt::getDeviceExtension(Device* device) {
    var key = (device->adapter()->getInfo()->id << 8) + device->type();
    var creator = (DEVICEEXTENSIONCREATOR*)PlayerExt::deviceExtensions_.get(key);
    return (*creator)(device);
}

void PlayerExt::addDeviceExtension(int key, DEVICEEXTENSIONCREATOR* deviceExtCreator) {
    PlayerExt::deviceExtensions_.add(key, deviceExtCreator);
}