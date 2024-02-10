#include "base/memory.h"
#include "math.h"
#include "synth/src/elem/dahr.h"
#include "synth/src/elem/adsr.h"

NS_FW_BASE_USE

using namespace SYNTH;

Dahr::Dahr() : Env() {
    rate2_ = 0.0;
    smp_ = 0.0;
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
    switch (phase_) {
        case EnvPhase::Up:
            smp_ = 0.0;
            timer_ = 0.0;
            rate_ = Env::attackRates[values_->del.b];
            phase_ = EnvPhase::Del;
        case EnvPhase::Del:
            timer_ += rate_;
            if (timer_ >= 1.0) {
                rate_ = Env::attackRates[values_->atk.b];
                phase_ = EnvPhase::Atk;
                timer_ = 0.0;
            }
            break;
        case EnvPhase::Atk:
            timer_ += rate_;
            if (timer_ >= 1.0) {
                rate_ = Env::attackRates[values_->hld.b];
                phase_ = EnvPhase::Hld;
                timer_ = 1.0;
            }
            smp_ = SMOOTH(timer_);
            break;
        case EnvPhase::Hld:
            smp_ = 1.0;
            timer_ -= rate_;
            if (timer_ < 0.0) {
                rate_ = Env::releaseRates[values_->rel.b];
                rate2_ = Env::releaseRatesExp[values_->rel.b];
                phase_ = EnvPhase::Rel;
                timer_ = 1.0;
            }
            break;
        case EnvPhase::Rel:
            timer_ -= rate_;
            if (timer_ < 0.0) {
                phase_ = EnvPhase::Idle;
                rate_ = 0.0;
                timer_ = 0.0;
                ai_[0] = ai_[1] = 0.0;
            } 
            smp_ *= rate2_;
            break;
    }
    ticks_++;
    //applyLPF();
    return (float)(values_->amp.f * smp_ * this->velocity_);
}
