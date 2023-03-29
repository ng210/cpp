#include <stdio.h>
#include "base/memory.h"
#include "math.h"
#include "synth/src/elem/adsr.h"

NS_FW_BASE_USE

using namespace SYNTH;

Adsr::Adsr() : Env(AdsrCtrlCount) {
    controls_ = NULL;
    gate_ = 0;
}

void Adsr::assignControls(PotBase* controls) {
    controls_ = controls;
    var ctrls = (AdsrCtrls*)controls_;
    ctrls->amp.init(0.0f, 1.0f, 0.01f, 0.5f);
    ctrls->dc.init(0.0f, 1.0f, 0.01f, 0.0f);
    ctrls->atk.init(0, 255, 1, 4);
    ctrls->dec.init(0, 255, 1, 16);
    ctrls->sus.init(0, 255, 1, 128);
    ctrls->rel.init(0, 255, 1, 24);
}

void Adsr::setFromStream(byte*& stream) {
    var ctrls = (AdsrCtrls*)controls_;
    ctrls->amp.setFromStream(stream);
    ctrls->dc.setFromStream(stream);
    ctrls->atk.setFromStream(stream);
    ctrls->dec.setFromStream(stream);
    ctrls->sus.setFromStream(stream);
    ctrls->rel.setFromStream(stream);
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
            timer_ = 1.0f;
            gate_ = 0;
        }
    }
}

float Adsr::run(Arg notUsed) {
    //var am = *((float*)params);
    double smp = 0.0;
    var ctrls = (AdsrCtrls*)controls_;
    var sus = ctrls->sus.value.f + 0.0000;
    switch (phase_) {
        case EnvPhase::Up: // atk precalc
            // 0.0 : 0.005s -> 1/(0*3.995 + 0.005)/smpRate = 200/smpRate
            // 1.0 : 4.0s -> 1/(1*3.995 + 0.005)/smpRate = 4/smpRate
            //   X : Xs -> 1/(3.995*X + 0.005)/smpRate
            rate_ = Env::attackRates[ctrls->atk.value.b];
            phase_ = EnvPhase::Atk;
        case EnvPhase::Atk: // atk
            //smp = SMOOTH(timer_);
            smp = timer_;
            timer_ += rate_;
            if (timer_ >= 1.0) {
                phase_ = EnvPhase::Dec;
                timer_ = 1.0;
            }
            break;
        case EnvPhase::Dec: // dec precalc
            // 0.0 : 0.005s -> 1/(0*3.995 + 0.005)/smpRate = 200/smpRate
            // 1.0 : 4.0s -> 1/(1*3.995 + 0.005)/smpRate
            //   X : Xs -> 1/(3.995*X + 0.005)/smpRate
            rate_ = Env::attackRates[ctrls->dec.value.b] * (1.0f - sus);
            phase_ = EnvPhase::Sus;
        case EnvPhase::Sus: // dec/sustain
            if (timer_ <= sus) {
                smp = timer_ = sus;
            } else {
                timer_ -= rate_;
                //var susm1 = 1.0 - sus;
                //smp = susm1 * SMOOTH((timer_ - sus)/susm1) + sus;
                smp = timer_;
            }
            break;
        case EnvPhase::Down: // rel precalc
            // 0.0 :  0.005s -> 1/(0*9.995 + 0.005)/smpRate = 200/smpRate
            // 1.0 : 10.0s -> 1/(1*9.995 + 0.005)/smpRate
            //   X :  Xs -> 1/(9.995*X + 0.005)/smpRate
            rate_ = Env::releaseRates[ctrls->rel.value.b];
            phase_ = EnvPhase::Rel;
        case EnvPhase::Rel: // rel
            //smp = ctrls->sus.value.f * SMOOTH(timer_ / sus);
            if (timer_ <= 0.0) {
                timer_ = 0.0;
                phase_ = EnvPhase::Idle; // set to idle
            }
            smp = timer_ * sus;
            timer_ -= rate_;
            break;
    }
    ticks_++;
    return (float)(ctrls->amp.value.f * smp * this->velocity_ + ctrls->dc.value.f);
}
