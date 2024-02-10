#ifndef __SONG_CTRL_H
#define __SONG_CTRL_H

#include "win/ctrl.h"
#include "synth/ui/track-ctrl.h"
#include "collection/array.h"

NS_FW_WIN_USE

namespace SYNTH_APP {
    class Song;

    class SongCtrl : public Ctrl {
        static WndClass wndClass_;

        PROP_R(Ctrl*, toolbar);
        PROP_R(Array, trackCtrls);
        PROP_R(Ctrl*, detailView);

        PROP_R(Song*, song);
    public:
        SongCtrl();
        virtual ~SongCtrl();

        void create(Window* parent, Song* song);
    };
}
#endif
