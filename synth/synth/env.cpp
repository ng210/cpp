#include "env.h"
#include "base/memory.h"

NS_FW_BASE_USE

using namespace SYNTH;

Env::Env() {
    controls_ = NULL;
    gate = 0;
    velocity = 0.0;
    phase_ = EnvPhase::Idle;
    timer = 0.0;
    rate = 0.0;
    ticks_ = 0;
}

void Env::assignControls(Pot* controls) {
    controls_ = (EnvCtrls*)controls;
    controls_->amp.init(0.0f, 1.0f, 0.01f, 1.0f);
    controls_->dc.init( 0.0f, 1.0f, 0.01f, 0.0f);
    controls_->atk.init(0, 255, 1,  4);
    controls_->dec.init(0, 255, 1, 16);
    controls_->sus.init(0, 255, 1,128);
    controls_->rel.init(0, 255, 1, 24);
}

void Env::setFromStream(byte* stream) {
    Mdl::setFromStream(stream, (Pot*)controls_);
    controls_->dc.setFromStream(stream);
    controls_->atk.setFromStream(stream);
    controls_->dec.setFromStream(stream);
    controls_->sus.setFromStream(stream);
    controls_->rel.setFromStream(stream);
}

void Env::setGate(byte v) {
    if (gate <= 0) {
        if (v > 0) {
            // slope up: retrigger envelope
            phase_ = EnvPhase::Up;
            timer = 0;
            ticks_ = 0;
            gate = 1;
            //velocity = v/255.0f;
        }            
    } else {
        if (v <= 0) {
            // slope down: start release phase
            phase_ = EnvPhase::Down;
            timer = controls_->sus.value_.f;
            gate = 0;
        }
    }
}

float Env::run(void* params) {
    var am = *((float*)params);
    switch (phase_) {
        case EnvPhase::Up: // atk precalc
            // 0.0 : 0.005s -> 1/(0*3.995 + 0.005)/smpRate = 200/smpRate
            // 1.0 : 4.0s -> 1/(1*3.995 + 0.005)/smpRate = 4/smpRate
            //   X : Xs -> 1/(3.995*X + 0.005)/smpRate
            rate = 1.0/(*samplingRate_ * (3.995 * controls_->atk.value_.f + 0.005));
            phase_ = EnvPhase::Atk;
        case EnvPhase::Atk: // atk
            timer += rate;
            if (timer >= 1.0) {
                phase_ = EnvPhase::Dec;
                timer = 1.0;
            }
            //smp = smooth(this.timer);
            break;
        case EnvPhase::Dec: // dec precalc
            // 0.0 : 0.005s -> 1/(0*3.995 + 0.005)/smpRate = 200/smpRate
            // 1.0 : 4.0s -> 1/(1*3.995 + 0.005)/smpRate
            //   X : Xs -> 1/(3.995*X + 0.005)/smpRate
            rate = 1.0/(*samplingRate_ * (3.995 * controls_->dec.value_.f + 0.005));
            phase_ = EnvPhase::Sus;
        case EnvPhase::Sus: // dec/sustain
            if (timer <= controls_->sus.value_.f) {
                timer = controls_->sus.value_.f;
            } else {
                timer -= rate;
                //var susm1 = 1- this.sus;
                //smp = susm1*smooth((this.timer-this.sus)/susm1) + this.sus;
            }
            break;
        case EnvPhase::Down: // rel precalc
            // 0.0 :  0.005s -> 1/(0*9.995 + 0.005)/smpRate = 200/smpRate
            // 1.0 : 10.0s -> 1/(1*9.995 + 0.005)/smpRate
            //   X :  Xs -> 1/(9.995*X + 0.005)/smpRate
            rate = timer/(*samplingRate_ * (9.995 * controls_->rel.value_.f + 0.005));
            phase_ = EnvPhase::Rel;
        case EnvPhase::Rel: // rel
            timer -= rate;
            if (timer <= 0.0) {
                timer = 0.0;
                phase_ = EnvPhase::Idle; // set to idle
            }
            //smp = this.sus*smooth(this.timer/this.sus);
            break;
    }
    ticks_++;
    return (float)(controls_->amp.value_.f * timer + controls_->dc.value_.f);
}

//Env.createControls = function createControls() {
//    return {
//        'amp': new psynth.PotF32(0, 1, .0),
//        'dc':  new psynth.PotF32(0, 1, .0),
//        'atk': new psynth.PotF8(0, 1, .0),
//        'dec': new psynth.PotF8(0, 1, .0),
//        'sus': new psynth.PotF8(0, 1, .0),
//        'rel': new psynth.PotF8(0, 1, .0)
//    }
//};
