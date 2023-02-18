#include <stdio.h>

#include "base/memory.h"
#include "math.h"
#include "synth/src/elem/adsr.h"

NS_FW_BASE_USE

using namespace SYNTH;

Adsr::Adsr() {
    controls_ = NULL;
    gate_ = 0;
}

void Adsr::assignControls(PotBase* controls) {
    controls_ = (AdsrCtrls*)controls;
    controls_->amp.init(0.0f, 1.0f, 0.01f, 0.5f);
    controls_->dc.init( 0.0f, 1.0f, 0.01f, 0.0f);
    controls_->atk.init(0, 255, 1,  4);
    controls_->dec.init(0, 255, 1, 16);
    controls_->sus.init(0, 255, 1,128);
    controls_->rel.init(0, 255, 1, 24);
}

void Adsr::setFromStream(byte*& stream) {
    controls_->amp.setFromStream(stream);
    controls_->dc.setFromStream(stream);
    controls_->atk.setFromStream(stream);
    controls_->dec.setFromStream(stream);
    controls_->sus.setFromStream(stream);
    controls_->rel.setFromStream(stream);
}

void Adsr::setGate(byte v) {
    if (gate_ <= 0) {
        if (v > 0) {
            // slope up: retrigger adsrelope
            phase_ = EnvPhase::Up;
            timer_ = 0;
            ticks_ = 0;
            gate_ = 1;
            velocity_ = v/255.0f;
        }            
    } else {
        if (v <= 0) {
            // slope down: start release phase
            phase_ = EnvPhase::Down;
            timer_ = controls_->sus.value.f;
            gate_ = 0;
        }
    }
}

float Adsr::run(Arg notUsed) {
    //var am = *((float*)params);
    double smp = 0.0;
    var sus = controls_->sus.value.f + 0.0001;
    switch (phase_) {
        case EnvPhase::Up: // atk precalc
            // 0.0 : 0.005s -> 1/(0*3.995 + 0.005)/smpRate = 200/smpRate
            // 1.0 : 4.0s -> 1/(1*3.995 + 0.005)/smpRate = 4/smpRate
            //   X : Xs -> 1/(3.995*X + 0.005)/smpRate
            rate_ = Env::attackRates[controls_->atk.value.b];
            phase_ = EnvPhase::Atk;
        case EnvPhase::Atk: // atk
            smp = SMOOTH(timer_);
            if (timer_ >= 1.0) {
                phase_ = EnvPhase::Dec;
                timer_ = 1.0;
            }
            timer_ += rate_;
            break;
        case EnvPhase::Dec: // dec precalc
            // 0.0 : 0.005s -> 1/(0*3.995 + 0.005)/smpRate = 200/smpRate
            // 1.0 : 4.0s -> 1/(1*3.995 + 0.005)/smpRate
            //   X : Xs -> 1/(3.995*X + 0.005)/smpRate
            rate_ = Env::attackRates[controls_->dec.value.b] * (1.0f - sus);
            phase_ = EnvPhase::Sus;
        case EnvPhase::Sus: // dec/sustain
            if (timer_ <= sus) {
                smp = timer_ = sus;
            } else {
                timer_ -= rate_;
                var susm1 = 1.0 - sus;
                smp = susm1 * SMOOTH((timer_ - sus)/susm1) + sus;
            }
            break;
        case EnvPhase::Down: // rel precalc
            // 0.0 :  0.005s -> 1/(0*9.995 + 0.005)/smpRate = 200/smpRate
            // 1.0 : 10.0s -> 1/(1*9.995 + 0.005)/smpRate
            //   X :  Xs -> 1/(9.995*X + 0.005)/smpRate
            rate_ = Env::releaseRates[controls_->rel.value.b] * sus;
            phase_ = EnvPhase::Rel;
        case EnvPhase::Rel: // rel
            smp = controls_->sus.value.f * SMOOTH(timer_ / sus);
            if (timer_ <= 0.0) {
                timer_ = 0.0;
                phase_ = EnvPhase::Idle; // set to idle
            }
            timer_ -= rate_;
            break;
    }
    ticks_++;
    return (float)(controls_->amp.value.f * smp * this->velocity_ + controls_->dc.value.f);
}
