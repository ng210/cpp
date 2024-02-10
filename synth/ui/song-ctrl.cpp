#include "synth/ui/song-ctrl.h"
#include "synth/ui/song.h"

using namespace SYNTH_APP;

WndClass SongCtrl::wndClass_;

SongCtrl::SongCtrl() {
    if (SongCtrl::wndClass_.atom == 0) {
        SongCtrl::wndClass_.atom = registerClass("SongCtrl", NULL, CS_OWNDC);
    }
    toolbar_ = NULL;
    trackCtrls_ = NULL;
    detailView_ = NULL;
}

SongCtrl::~SongCtrl() {
    DEL_(toolbar_);
    DEL_(detailView_);
    trackCtrls_.apply(
        [](COLLECTION_ARGUMENTS) {
            DEL_(value);
            return value;
        });
}

void SongCtrl::create(Window* parent, Song* song) {
	Ctrl::create(SongCtrl::wndClass_, parent, song->name());
    toolbar_ = NULL;        // NEW_(Toolbar)
    trackCtrls_.init(sizeof(TrackCtrl), 16);
    detailView_ = NULL;     // NEW_(DetailView)
	song_ = song;

}

