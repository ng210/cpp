#include "base/memory.h"
#include "math.h"
#include "synth/src/elem/dahr.h"
#include "synth/src/elem/adsr.h"

NS_FW_BASE_USE

using namespace SYNTH;

Dahr::Dahr() : Env() {
    smp_ = 0.0;
    values_ = NULL;
}

DahrValues* Dahr::values() {
    return (DahrValues*)values_;
}

void Dahr::setGate(byte v) {
    if (v > 0) {
        // slope up: retrigger envelope
        phase_ = EnvPhase::Up;
        ticks_ = 0;
        velocity_ = v / 255.0f;
        overlaySmp_ = smp_;
        timer_ = 0.0;
    }
}

float Dahr::run(Arg notUsed) {
    if (phase_ != EnvPhase::Idle) {
        timer_ += rate_;
        switch (phase_) {
        case EnvPhase::Up:
            rate_ = Env::attackRates[values()->del.b];
            phase_ = EnvPhase::Del;
        case EnvPhase::Del:
            if (timer_ >= 1.0) {
                rate_ = Env::attackRates[values()->atk.b];
                phase_ = EnvPhase::Atk;
                timer_ = 0.0;
            }
            smp_ = 0.0;
            break;
        case EnvPhase::Atk:
            if (timer_ < 1.0) {
                smp_ = SMOOTH(timer_);
            }
            else {
                rate_ = Env::attackRates[values()->hld.b];
                phase_ = EnvPhase::Hld;
                timer_ = 0.0;
            }
            break;
        case EnvPhase::Hld:
            if (timer_ >= 1.0) {
                rate_ = -Env::releaseRates[values()->rel.b];
                phase_ = EnvPhase::Rel;
                timer_ = 1.0;
            }
            break;
        case EnvPhase::Rel:
            if (timer_ <= 0.0) {
                phase_ = EnvPhase::Idle;
                rate_ = 0.0;
                timer_ = 0.0;
            }
            smp_ = SMOOTH(timer_);
            break;
        case EnvPhase::Kill:
            if (timer_ <= 0.0) {
                phase_ = EnvPhase::Idle;
            }
            smp_ = SMOOTH(timer_);
            break;
        }

        //applyLPF();
    }
    ticks_++;
    return (float)(values()->amp.f * smp_ * this->velocity_);
}
