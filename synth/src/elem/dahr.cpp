#include "base/memory.h"
#include "math.h"
#include "synth/src/elem/dahr.h"
#include "synth/src/elem/adsr.h"

NS_FW_BASE_USE

using namespace SYNTH;

Dahr::Dahr() {
    controls_ = NULL;
    rate2_ = 0.0;
    smp_ = 0.0;
}

void Dahr::assignControls(PotBase* controls) {
    controls_ = (DahrCtrls*)controls;
    controls_->amp.init(0.0f, 1.0f, 0.01f, 0.0f);
    controls_->dc.init( 0.0f, 1.0f, 0.01f, 0.0f);
    controls_->del.init(0, 255, 1, 16);
    controls_->atk.init(0, 255, 1, 4);
    controls_->hld.init(0, 255, 1, 16);
    controls_->rel.init(0, 255, 1, 80);
}

void Dahr::setFromStream(byte*& stream) {
    controls_->amp.setFromStream(stream);
    controls_->dc.setFromStream(stream);
    controls_->del.setFromStream(stream);
    controls_->atk.setFromStream(stream);
    controls_->hld.setFromStream(stream);
    controls_->rel.setFromStream(stream);
}

void Dahr::setGate(byte v) {
    if (v > 0) {
        // slope up: retrigger envelope
        phase_ = EnvPhase::Up;
        timer_ = 0.0;
        ticks_ = 0;
        velocity_ = v / 255.0f;
        rate_ = 0.0;
    }
    else {
        phase_ = EnvPhase::Hld;
        timer_ = -1.0;
    }
}

float Dahr::run(Arg notUsed) {
    //var am = *((float*)params);
    switch (phase_) {
        case EnvPhase::Up:
            smp_ = 0.0;
            timer_ = 0.0;
            rate_ = Env::attackRates[controls_->del.value.b];
            phase_ = EnvPhase::Del;
        case EnvPhase::Del:
            timer_ += rate_;
            if (timer_ >= 1.0) {
                rate_ = Env::attackRates[controls_->atk.value.b];
                phase_ = EnvPhase::Atk;
                timer_ = 0.0;
            }
            break;
        case EnvPhase::Atk:
            timer_ += rate_;
            if (timer_ >= 1.0) {
                rate_ = Env::attackRates[controls_->hld.value.b];
                phase_ = EnvPhase::Hld;
                timer_ = 1.0;
            }
            smp_ = SMOOTH(timer_);
            break;
        case EnvPhase::Hld:
            timer_ -= rate_;
            if (timer_ < 0.0) {
                rate_ = Env::releaseRates[controls_->rel.value.b];
                rate2_ = Env::releaseRatesExp[controls_->rel.value.b];
                phase_ = EnvPhase::Rel;
                timer_ = 1.0;
            }
            smp_ = 1.0;
            break;
        case EnvPhase::Rel:
            timer_ -= rate_;
            if (timer_ < 0.0) {
                phase_ = EnvPhase::Idle;
                rate_ = 0.0;
                timer_ = 0.0;
            }
            smp_ *= rate2_;
            break;
    }
    ticks_++;
    return (float)(controls_->amp.value.f * smp_ * this->velocity_ + controls_->dc.value.f);
}
