#include "channel-ctrl.h"

using namespace SYNTH_APP;

WndClass ChannelCtrl::wndClass_;

ChannelCtrl::ChannelCtrl() {
    if (ChannelCtrl::wndClass_.atom == 0) {
        ChannelCtrl::wndClass_.atom = registerClass("ChannelCtrl", NULL, CS_OWNDC);
    }
    channel_ = NULL;
    detailsCtrl_ = NULL;
    sequenceCtrl_ = NEW_(SequenceCtrl);

}

ChannelCtrl::~ChannelCtrl() {

}

void ChannelCtrl::create(Window* parent, Channel* channel) {
    var name = (char*)channel->id();
    Ctrl::create(ChannelCtrl::wndClass_, parent, name);
    //detailsCtrl_->create(parent, name);
    sequenceCtrl_->create(parent, name);
    channel_ = channel;
}

