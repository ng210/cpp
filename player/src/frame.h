#ifndef __PLAYER_FRAME_H
#define __PLAYER_FRAME_H

#include "collection/parray.h"

NS_FW_BASE_USE

namespace PLAYER {

    typedef struct Frame {
        word delta_;
        PArray commands_;
        inline Frame* addCommand(byte* command) {
            commands_.add(command);
            return this;
        }
    } Frame;
}

#endif