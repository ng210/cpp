#include "env.h"
#include "base/memory.h"
#include "math.h"

NS_FW_BASE_USE

using namespace SYNTH;

float attackRates[256];
float releaseRates[256];

Env::Env() {
    controls_ = NULL;
    gate = 0;
    velocity = 0.0;
    phase_ = EnvPhase::Idle;
    timer = 0.0;
    rate = 0.0;
    ticks_ = 0;
}

void Env::assignControls(PotBase* controls) {
    controls_ = (EnvCtrls*)controls;
    controls_->amp.init(0.0f, 1.0f, 0.01f, 0.0f);
    controls_->dc.init( 0.0f, 1.0f, 0.01f, 0.0f);
    controls_->atk.init(0, 255, 1,  4);
    controls_->dec.init(0, 255, 1, 16);
    controls_->sus.init(0, 255, 1,128);
    controls_->rel.init(0, 255, 1, 24);
}

void Env::setFromStream(byte* stream) {
    controls_->amp.setFromStream(stream);
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
            velocity = v/255.0f;
        }            
    } else {
        if (v <= 0) {
            // slope down: start release phase
            phase_ = EnvPhase::Down;
            timer = controls_->sus.value.f;
            gate = 0;
        }
    }
}

float Env::run(Arg notUsed) {
    //var am = *((float*)params);
    switch (phase_) {
        case EnvPhase::Up: // atk precalc
            // 0.0 : 0.005s -> 1/(0*3.995 + 0.005)/smpRate = 200/smpRate
            // 1.0 : 4.0s -> 1/(1*3.995 + 0.005)/smpRate = 4/smpRate
            //   X : Xs -> 1/(3.995*X + 0.005)/smpRate
            rate = attackRates[controls_->atk.value.b];
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
            rate = attackRates[controls_->dec.value.b];
            phase_ = EnvPhase::Sus;
        case EnvPhase::Sus: // dec/sustain
            if (timer <= controls_->sus.value.f) {
                timer = controls_->sus.value.f;
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
            rate = releaseRates[controls_->rel.value.b];
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
    return (float)(controls_->amp.value.f * timer * this->velocity + controls_->dc.value.f);
}


void Env::initialize(float samplingRate) {
    // calculate rates
    int steps = 255;
    float px = 0.8f;
    float py = 1.0f - px;
    float ax = 1.0f - 2.0f * px, ay = 1.0f - 2.0f * py;
    float bx = 2.0f * px, by = 2.0f * py;
    for (auto i = 0; i <= steps; i++) {
        float x = (float)i / steps;
        float r = (-bx + (float)sqrt(bx * bx + 4.0f * ax * x)) / (2.0f * ax);
        float y = r * r * ay + by * r;
        attackRates[i] = 1.0f / (samplingRate * (0.005f + 0.995f * y));
        releaseRates[i] = 1.0f / (samplingRate * (0.005f + 3.995f * y));
    }
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
