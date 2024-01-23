#ifndef __PLAYER_FRAME_H
#define __PLAYER_FRAME_H

#include "collection/map.h"
#include "collection/parray.h"
#include "player/src/sequence.h"
#include "player/src/device-ext.h"

NS_FW_BASE_USE

namespace PLAYER {

    typedef int(FRAMES_GET_CHANNEL_ID)(UINT8*&);

    typedef struct Frame {
        word time;
        PArray commands;
        Frame(int t) : time(t) {}
    } Frame;

    typedef KeyValuePair* (FRAME_FILTER)(int time, byte* command, va_list args);

    class FrameList : public PArray {
        PROP(DeviceExt*, deviceExt);
        int insertCommand(int time, byte* command);
        void removeCommand(int time, byte* command);
    public:
        FrameList(DeviceExt* deviceExt = NULL);
        virtual ~FrameList();

        Sequence* toSequence(DeviceExt* deviceExt);
        char* toJSON(DeviceExt* deviceExt);

        static FrameList* fromSequence(Sequence* seq, DeviceExt* deviceExt);
        // Map<key, FrameList*>
        static Map* splitSequence(Sequence* seq, DeviceExt* deviceExt, FRAME_FILTER* filter, ...);
        static Sequence* merge(Map* frameListMap, DeviceExt* deviceExt);
    
    private:
        static COLLECTION_COMPARE compareFrames;
        static COLLECTION_COMPARE compareCommands;
    };
}

#endif