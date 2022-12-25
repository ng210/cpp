#include "math.h"
#include "clp.h"

NS_FW_BASE_USE

using namespace SYNTH;

Clp::Clp() {
    controls_ = NULL;
}

Clp::~Clp() {

}

void Clp::assignControls(PotBase* controls) {
    controls_ = (ClpCtrls*)controls;
    controls_->amp.init(0.0f, 1.0f, 0.01f, 1.0f);
    controls_->lvl.init(0.0f, 1.0f, 0.01f, 1.0f);
}

void Clp::setFromStream(byte* stream) {

}

float Clp::run(Arg params) {
    var smp = params.f * controls_->amp.value.f;
    if (smp < -controls_->lvl.value.f) smp = -controls_->lvl.value.f;
    else if (smp > controls_->lvl.value.f) smp = controls_->lvl.value.f;
    return smp;
}