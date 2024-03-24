#include <stdio.h>
#include "basedef.h"
#include "math.h"
#include "synth/src/elem/adsr.h"

NS_FW_BASE_USE

using namespace SYNTH;

Adsr::Adsr() : Env() {
    gate_ = 0;
    values_ = NULL;
}

AdsrValues* Adsr::values() {
    return (AdsrValues*)values_;
}

void Adsr::setGate(byte v) {
    if (gate_ <= 0) {
        if (v > 0) {
            // slope up: retrigger envelope
            phase_ = EnvPhase::Up;
            ticks_ = 0;
            gate_ = 1;
            velocity_ = v / 255.0f;
            overlaySmp_ = smp_;
            timer_ = 0.0;
        }
    }
    else {
        if (v <= 0) {
            // slope down: start release phase
            phase_ = EnvPhase::Down;
            gate_ = 0;
        }
    }
}

float Adsr::run(Arg notUsed) {
    if (phase_ != EnvPhase::Idle) {
        var sus = values()->sus.f + 0.0000;
        switch (phase_) {
        case EnvPhase::Up: // atk precalc
            rate_ = Env::attackRates[values()->atk.b];
            phase_ = EnvPhase::Atk;
        case EnvPhase::Atk: // atk
            timer_ += rate_;
            if (timer_ >= 1.0) {
                timer_ = 1.0;
                rate_ = -Env::attackRates[values()->dec.b] * (1.0f - sus);
                phase_ = EnvPhase::Sus;
            }
            break;
        case EnvPhase::Sus: // dec/sustain
            timer_ += rate_;
            if (timer_ <= sus) {
                timer_ = sus;
            }
            break;
        case EnvPhase::Down: // rel precalc
            rate_ = -Env::releaseRates[values()->rel.b] * timer_;
            phase_ = EnvPhase::Rel;
        case EnvPhase::Rel: // rel
            timer_ += rate_;
            if (timer_ <= 0.0) {
                timer_ = 0.0;
                phase_ = EnvPhase::Idle; // set to idle
                rate_ = 0.0;
            }
            break;
        case EnvPhase::Kill:
            timer_ += rate_;
            if (timer_ <= 0.0) {
                phase_ = EnvPhase::Idle;
                timer_ = 0.0;
            }
            break;
        }

        smp_ = SMOOTH(timer_);
        //applyLPF();
    }
    ticks_++;
    return (float)(values()->amp.f * smp_ * this->velocity_);
}
