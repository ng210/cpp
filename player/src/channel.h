#ifndef __PLAYER_CHANNEL_H
#define __PLAYER_CHANNEL_H

#include "basedef.h"
#include "frame.h"
#include "device.h"
#include "adapter.h"
#include "sequence.h"

NS_FW_BASE_USE
namespace PLAYER {

    class Channel : public Device {
    private: char id_[16];
    private: PROP(Sequence*, sequence);
    private: PROP(PArray, frames);
    private: PROP(Device*, device);
    private: PROP(Adapter*, adapter);
    private: PROP(bool, isActive);
    private: PROP(int, currentTick);
    private: PROP(int, loopCount);
    private: PROP(byte*, cursor);
    public:
        Channel(char* id);
        virtual ~Channel();
        void assign(int deviceId, Sequence* sequence);
        void reset();
        bool run(int ticks);
        void setRefreshRate(float fps) {}

        inline const char* id() { return id_; }
        void id(char* str);

        PArray* toFrames();
        Sequence* toSequence();
    };
}
#endif