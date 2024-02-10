#include "math.h"
#include "clp.h"

NS_FW_BASE_USE

using namespace SYNTH;

Clp::Clp() {
}

float Clp::run(Arg params) {
    var smp = params.f * values_->amp.f;
    if (smp < -values_->lvl.f) smp = -values_->lvl.f;
    else if (smp > values_->lvl.f) smp = values_->lvl.f;
    return smp;
}
