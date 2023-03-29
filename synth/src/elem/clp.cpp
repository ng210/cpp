#include "math.h"
#include "clp.h"

NS_FW_BASE_USE

using namespace SYNTH;

Clp::Clp() : Elem(ClpCtrlsCount) {
    controls_ = NULL;
}

Clp::~Clp() {

}

void Clp::assignControls(PotBase* controls) {
    controls_ = controls;
    var ctrls = (ClpCtrls*)controls_;
    ctrls->amp.init(0.0f, 10.0f, 0.01f, 1.0f);
    ctrls->lvl.init(0, 255, 1, 240);
}

void Clp::setFromStream(byte*& stream) {

}

float Clp::run(Arg params) {
    var ctrls = (ClpCtrls*)controls_;
    var smp = params.f * ctrls->amp.value.f;
    if (smp < -ctrls->lvl.value.f) smp = -ctrls->lvl.value.f;
    else if (smp > ctrls->lvl.value.f) smp = ctrls->lvl.value.f;
    return smp;
}
