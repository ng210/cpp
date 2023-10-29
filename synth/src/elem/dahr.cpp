#include "base/memory.h"
#include "math.h"
#include "synth/src/elem/dahr.h"
#include "synth/src/elem/adsr.h"

NS_FW_BASE_USE

using namespace SYNTH;

Dahr::Dahr() : Env(DahrCtrlCount) {
    controls_ = NULL;
    rate2_ = 0.0;
    smp_ = 0.0;
}

//void Dahr::assignControls(PotBase* controls) {
//    controls_ = controls;
//    var ctrls = (DahrCtrls*)controls_;
//    ctrls->amp.init(0.0f, 1.0f, 0.01f, 0.0f);
//    //ctrls->dc.init(0.0f, 1.0f, 0.01f, 0.0f);
//    ctrls->del.init(0, 255, 1, 16);
//    ctrls->atk.init(0, 255, 1, 4);
//    ctrls->hld.init(0, 255, 1, 16);
//    ctrls->rel.init(0, 255, 1, 80);
//}
//
//void Dahr::setFromStream(byte*& stream) {
//    var ctrls = (DahrCtrls*)controls_;
//    ctrls->amp.setFromStream(stream);
//    //ctrls->dc.setFromStream(stream);
//    ctrls->del.setFromStream(stream);
//    ctrls->atk.setFromStream(stream);
//    ctrls->hld.setFromStream(stream);
//    ctrls->rel.setFromStream(stream);
//}

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
    var ctrls = (DahrCtrls*)controls_;
    switch (phase_) {
        case EnvPhase::Up:
            smp_ = 0.0;
            timer_ = 0.0;
            rate_ = Env::attackRates[ctrls->del.value.b];
            phase_ = EnvPhase::Del;
        case EnvPhase::Del:
            timer_ += rate_;
            if (timer_ >= 1.0) {
                rate_ = Env::attackRates[ctrls->atk.value.b];
                phase_ = EnvPhase::Atk;
                timer_ = 0.0;
            }
            break;
        case EnvPhase::Atk:
            timer_ += rate_;
            if (timer_ >= 1.0) {
                rate_ = Env::attackRates[ctrls->hld.value.b];
                phase_ = EnvPhase::Hld;
                timer_ = 1.0;
            }
            smp_ = SMOOTH(timer_);
            break;
        case EnvPhase::Hld:
            timer_ -= rate_;
            if (timer_ < 0.0) {
                rate_ = Env::releaseRates[ctrls->rel.value.b];
                rate2_ = Env::releaseRatesExp[ctrls->rel.value.b];
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
                ai_[0] = ai_[1] = 0.0;
            }
            smp_ *= rate2_;
            break;
    }
    ticks_++;
    //applyLPF();
    return (float)(ctrls->amp.value.f * smp_ * this->velocity_);
}
