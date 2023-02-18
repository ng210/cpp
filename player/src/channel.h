#ifndef __PLAYER_CHANNEL_H
#define __PLAYER_CHANNEL_H

#include "basedef.h"
#include "player/src/device.h"
#include "player/src/sequence.h"

NS_FW_BASE_USE
namespace PLAYER {

    class Channel {
    protected: char id_[16];
    protected: PROP(Sequence*, sequence);
    //private: PROP(PArray, frames);
    protected: PROP(Device*, device);
    protected: PROP(int, currentTick);
    protected: PROP(int, loopCount);
    protected: PROP(byte*, cursor);
    protected: PROP(bool, isActive);
    protected: PROP(bool, isEndless);

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