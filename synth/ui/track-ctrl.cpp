#include "synth/ui/track-ctrl.h"
#include "player/src/adapter.h"
#include "synth/ui/device-ctrl.h"

using namespace SYNTH_APP;

WndClass TrackCtrl::wndClass_;

TrackCtrl::TrackCtrl() {
    if (TrackCtrl::wndClass_.atom == 0) {
        TrackCtrl::wndClass_.atom = registerClass("TrackCtrl", NULL, CS_OWNDC);
    }

    deviceCtrl_ = NULL;
    sequenceCtrl_ = NEW_(SequenceCtrl);

}

TrackCtrl::~TrackCtrl() {

}

void TrackCtrl::create(Window* parent, Device* device) {
    var name = fmw::strdup("Track");
    Ctrl::create(TrackCtrl::wndClass_, parent, name);
    device_ = device;
    deviceCtrl_ = DeviceCtrl::create(device, this, name);
    //detailsCtrl_->create(parent, name);
    sequenceCtrl_->create(this, name);
}

