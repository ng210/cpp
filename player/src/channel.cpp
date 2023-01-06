#include "base/memory.h"
#include "channel.h"
#include "player-device.h"
//#include "player-adapter.h"


NS_FW_BASE_USE
using namespace PLAYER;

Channel::Channel() {
    const char* id = "ChannelXX";
    initialize((byte**)&id);
}

Channel::Channel(char* idStr) {
    initialize((byte**)&idStr);
}

Channel::~Channel() {
    ;
}

void Channel::initialize(byte** pData) {
    if (pData != NULL && *pData != NULL) {
        byte*& data = *pData;
        var i = 0;
        for (; i < 16; i++) {
            var ch = *data++;
            if (ch == '\0') break;
            id_[i] = ch;
        }
        id_[i] = '\0';
    }    
    sequence_ = NULL;
    device_ = NULL;
    isActive_ = false;
    currentTick_ = 0;
    loopCount_ = 1;
    cursor_ = (byte*)1;
}

int Channel::run(int ticks) {
    while (isActive_) {
        var isRestarted = false;
        var cursor = cursor_;
        //var delta = READ(cursor, word);
        word delta;
        while ((delta = *(word*)cursor_) <= currentTick_) {
            cursor_ += sizeof(word);
            currentTick_ -= delta;
            byte cmd = -1;
            while (true) {
                // read command code, 1 byte
                cmd = READ(cursor_, byte);
                if (cmd > 1) {
                    device_->processCommand(cmd, cursor_);   //this.device, cmd, this.sequence, this.cursor);
                }
                else {
                    if (cmd == PlayerCommands::CmdEOS) {
                        // end of sequence
                        reset();
                        isRestarted = isActive_;
                    }
                    // cmd == 0: end of frame
                    break;
                }
            }
            if (cmd == PlayerCommands::CmdEOS) {
                // end of sequence
                //this.currentTick = 0;
                break;
            }
            //delta = READ(cursor, word);
        }
        if (isRestarted) {
            // end of sequence
            continue;
        }
        currentTick_ += ticks;
        break;
    }
    return isActive_;
}

void Channel::assign(Device* device, Sequence* sequence, int loopCount) {
    sequence_ = sequence;

    cursor_ = sequence_->data() + 1;
    device_ = device;
    loopCount_ = loopCount;
    //this.currentTick = 0;
    isActive_ = true;
    run(0);
}

void Channel::reset() {
    cursor_ = sequence_->data() + 1;
    if (--loopCount_ > 0) {
        currentTick_ = 0;
        isActive_ = true;
    }
    else {
        isActive_ = false;
    }
}

void Channel::id(char* str) {
    strncpy(id_, 15, str);
}


//PArray* Channel::toFrames() {
//    return sequence_->toFrames();
//}

//Sequence* Channel::toSequence() {
//    return Sequence::fromFrames((Collection*)&frames_, device_);
//}
