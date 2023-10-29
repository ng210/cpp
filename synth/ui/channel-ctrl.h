#ifndef __CHANNEL_CTR_H
#define __CHANNEL_CTR_H

#include "win/ctrl.h"
#include "synth/ui/sequence-ctrl.h"
#include "player/src/channel.h"

using namespace PLAYER;
namespace SYNTH_APP {

    class ChannelCtrl : public Ctrl {
        static WndClass wndClass_;

        PROP_R(Ctrl*, detailsCtrl);
        PROP_R(SequenceCtrl*, sequenceCtrl);
        PROP_R(Channel*, channel);
    public:
        ChannelCtrl();
        virtual ~ChannelCtrl();

        void create(Window* parent, Channel* channel);
    };

}

#endif