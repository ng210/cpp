#ifndef __PLAYER_CHANNEL_H
#define __PLAYER_CHANNEL_H

#include "basedef.h"
#include "player/src/device.h"
#include "player/src/sequence.h"

NS_FW_BASE_USE
namespace PLAYER {

    class Channel {
    private: char id_[16];
    private: PROP(Sequence*, sequence);
    //private: PROP(PArray, frames);
    private: PROP(Device*, device);
    private: PROP(int, currentTick);
    private: PROP(int, loopCount);
    private: PROP(byte*, cursor);
    private: PROP(bool, isActive);

    public:

        inline const char* id() { return id_; }
        void id(char* str);

        Channel(char* id);
        Channel();
        virtual ~Channel();

        void initialize(byte** data);

        int run(int ticks);
        void assign(Device* device, Sequence* sequence, int loopCount = 0);
        void reset();

        //PArray* toFrames();
        //Sequence* toSequence();
    };
}
#endif