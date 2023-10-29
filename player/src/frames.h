#ifndef __PLAYER_FRAME_H
#define __PLAYER_FRAME_H

#include "collection/map.h"
#include "collection/parray.h"
#include "player/src/sequence.h"

NS_FW_BASE_USE

namespace PLAYER {

    typedef int(FRAMES_GET_CHANNEL_ID)(UINT8*&);

    typedef struct Command {
        word time;
        byte* command;
        Command() : time(0), command(NULL) {}
        Command(int time, byte* command) : time(time), command(command) {}
    } Command;

    class Frames {
        // Map<commandId, List<Command>>
        PROP_R(Map, channels);
        Device* device_;
    public:
        Frames();
        ~Frames();

        byte* insertCommand(Key time, Key channelId, byte* command);
        byte* removeCommand(Key time, Key channelId);
        Sequence* Frames::toSequence();

        //PArray getCommands(int start, int end, COLLECTION_COMPARE compare);

        static Frames* fromSequence(Sequence* sequence, FRAMES_GET_CHANNEL_ID getChannelId);
        static COLLECTION_COMPARE compareCommand;
    };
}

#endif