#include "lfo.h"
#include "lfo.h"
#include "math.h"

NS_FW_BASE_USE
using namespace SYNTH;

Lfo::Lfo() {
    reset();
}

void Lfo::reset() {
    timer = 0.0;
}

float Lfo::run(Arg params) {
    var smp = sin(SYNTH_THETA * timer);
    var delta = values_->fre.f / *Elem::samplingRate;
    if (delta >= 1.0f) {
        delta = 0.99999999f;
    }
    timer += delta;
    if (timer > 1.0) {
        timer -= 1.0;
    }

    return (float)(values_->amp.f * smp/* + controls_->dc.value.f*/);
}
