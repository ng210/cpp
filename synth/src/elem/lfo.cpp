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

void Lfo::assignControls(PotBase* controls) {
    controls_ = (LfoCtrls*)controls;
    controls_->amp.init(0.0f, 1.0f, 0.01f, 1.0f);
    //controls_->dc.init(0.0f, 1.0f, 0.01f, 0.0f);
    controls_->fre.init(0.0f, 20.0f, 0.01f, 2.0f);
}

void Lfo::setFromStream(byte* stream) {
    Elem::setFromStream(stream, (Pot*)controls_);
    //controls_->dc.setFromStream(stream);
    controls_->fre.setFromStream(stream);
}

float Lfo::run(Arg params) {
    var smp = sin(SYNTH_THETA * timer);
    var delta = controls_->fre.value.f / *samplingRate_;
    if (delta >= 1.0) {
        delta = 0.99999999f;
    }
    timer += delta;
    if (timer > 1.0) {
        timer -= 1.0;
    }

    return (float)(controls_->amp.value.f * smp/* + controls_->dc.value.f*/);
}
