#ifndef __TRACK_CTR_H
#define __TRACK_CTR_H

#include "win/ctrl.h"
#include "synth/ui/sequence-ctrl.h"
#include "synth/ui/track.h"

using namespace PLAYER;
namespace SYNTH_APP {

    class TrackCtrl : public Ctrl {
        static WndClass wndClass_;

        PROP_R(Track*, track);

        PROP_R(Ctrl*, deviceCtrl);
        PROP_R(SequenceCtrl*, sequenceCtrl);
        
    public:
        TrackCtrl();
        virtual ~TrackCtrl();

        void create(Window* parent, Device* device);
    };

}

#endif