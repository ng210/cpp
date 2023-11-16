#ifndef __PLAYER_FRAME_H
#define __PLAYER_FRAME_H

#include "collection/map.h"
#include "collection/parray.h"
#include "player/src/sequence.h"

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
        PROP(Device*, device);
        int insertCommand(int time, byte* command);
        void removeCommand(int time, byte* command);
    public:
        FrameList();
        FrameList(Device*);
        virtual ~FrameList();

        Sequence* toSequence(Device* device);
        char* toJSON();

        static FrameList* fromSequence(Sequence* seq);
        // Map<key, FrameList*>
        static Map* splitSequence(Sequence* seq, FRAME_FILTER* filter, ...);
        static Sequence* merge(Map* frameListMap, Device* device);
    
    private:
        static COLLECTION_COMPARE compareFrames;
        static COLLECTION_COMPARE compareCommands;
    };
}

#endif