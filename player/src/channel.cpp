#include "base/memory.h"
#include "channel.h"
#include "player-device.h"
//#include "player-adapter.h"


NS_FW_BASE_USE
using namespace PLAYER;

Channel::Channel() {
    const char* id = "ChannelXX";
    isEndless_ = false;
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
    currentFrame_ = 0;
    run_ = &Channel::runOnSequence;
}

void Channel::assign(Device* device, Sequence* sequence, int loopCount) {
    sequence_ = sequence;
    cursor_ = sequence_->data() + 1;
    device_ = device;
    loopCount_ = loopCount;
    currentTick_ = 0;
    isActive_ = true;
    run(0);
}

void Channel::assign(Device* device, FrameList* frames, int loopCount) {
    frames_ = frames;
    currentFrame_ = 0;
    device_ = device;
    loopCount_ = loopCount;
    currentTick_ = 0;
    isActive_ = true;
    run_ = &Channel::runOnFrameList;
    run(0);
}

int Channel::run(int ticks) {
    return (this->*run_)(ticks);
}

void Channel::reset() {
    if (sequence_) {
        cursor_ = sequence_->data() + 1;
    }
    currentFrame_ = 0;

    if (isEndless_ || --loopCount_ > 0) {
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

int Channel::runOnSequence(int ticks) {
    while (this->isActive_) {
        var isRestarted = false;
        word delta;
        while ((delta = *(word*)this->cursor_) <= this->currentTick_) {
            this->cursor_ += sizeof(word);
            this->currentTick_ -= delta;
            byte cmd = -1;
            while (true) {
                // read command code, 1 byte
                cmd = READ(this->cursor_, byte);
                if (cmd > 1) {
                    this->device_->processCommand(cmd, this->cursor_);
                }
                else {
                    if (cmd == PlayerCommands::CmdEOS) {
                        // end of sequence
                        this->reset();
                        isRestarted = this->isActive_;
                    }
                    // cmd == 0: end of frame
                    break;
                }
            }
            if (cmd == PlayerCommands::CmdEOS) {
                // end of sequence
                break;
            }
        }
        if (isRestarted) {
            // end of sequence
            continue;
        }
        this->currentTick_ += ticks;
        break;
    }
    return this->isActive_;
}

int Channel::runOnFrameList(int ticks) {
    while (this->isActive_) {
        var isRestarted = false;
        byte cmdId = 0;
        while (true) {
            var frame = (Frame*)this->frames_->get(this->currentFrame_);
            if (frame->time > this->currentTick_) break;
            for (var ci = 0; ci < frame->commands.length(); ci++) {
                var cmd = (byte*)frame->commands.get(ci);
                cmdId = READ(cmd, byte);
                if (cmdId > PlayerCommands::CmdEOS) {
                    this->device_->processCommand(cmdId, cmd);
                }
                else {
                    if (cmdId == PlayerCommands::CmdEOS) {
                        // end of sequence
                        this->reset();
                        isRestarted = this->isActive_;
                    }
                    // cmd == 0: end of frame
                    break;
                }
            }
            if (cmdId == PlayerCommands::CmdEOS) {
                // end of sequence
                break;
            }
            this->currentFrame_++;
            if (this->currentFrame_ == this->frames_->length()) {
                // error!!
                this->isActive(false);
            }
        }
        if (isRestarted) {
            // end of sequence
            continue;
        }
        this->currentTick_ += ticks;
        break;
    }
    return this->isActive_;

}