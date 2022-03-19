#include "channel.h"
#include "player.h"
#include "base/memory.h"

NS_FW_BASE_USE
using namespace PLAYER;

Channel::Channel(char* idStr) {
    id(idStr);
    sequence_ = NULL;
    device_ = NULL;
    adapter_ = NULL;
    isActive_ = false;
    currentTick_ = 0;
    loopCount_ = 1;
    cursor_ = (byte*)1;

}

Channel::~Channel() {
    ;
}

void Channel::assign(int deviceId, Sequence* sequence) {
    sequence_ = sequence;
    cursor_ = sequence->data() + 1;
    adapter_ = sequence->adapter();
    device_ = adapter_->getDevice(deviceId);
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
    //var i = 0;
    //while (id_[i] = str[i] != '\0' && i < 15) i++;
    //id_[i] = '\0';
}

bool Channel::run(int ticks) {
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
                    adapter_->processCommand(this, cmd);   //this.device, cmd, this.sequence, this.cursor);
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

PArray* Channel::toFrames() {
    return sequence_->toFrames();
}

Sequence* Channel::toSequence() {
    return Sequence::fromFrames((Collection*)&frames_, adapter_);
}
