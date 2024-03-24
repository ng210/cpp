#define _USE_MATH_DEFINES

#include "synth/src/elem/env.h"
#include "math.h"

using namespace SYNTH;

double Env::attackRates[256];
double Env::releaseRates[256];
double Env::releaseRatesExp[256];
double Env::fadoutFactor;

Env::Env() {
    velocity_ = 0.0;
    ticks_ = 0;
    timer_ = 0.0;
    rate_ = 0.0;
    phase_ = EnvPhase::Idle;
    smp_ = 0.0;
    //// LPF
    //fc_ = M_PI * 0.1;
    //ai_[0] = ai_[1] = 0.0;
    overlaySmp_ = 0.0;
}

void Env::kill() {
    if (phase_ != EnvPhase::Idle) {
        phase_ = EnvPhase::Kill;
        rate_ = -Env::attackRates[10];
    }
}

void Env::setValues(void* v) {
    values_ = (Value*)v;
}

//void Env::applyLPF() {
//    ai_[0] = ai_[0] + fc_ * (smp_ - ai_[0]);
//    ai_[1] = ai_[1] + fc_ * (ai_[0] - ai_[1]);
//    smp_ = (float)ai_[1];
//}

void Env::initialize() {
    // calculate rates
    int steps = 255;
    double px = 0.8;
    double py = 1.0 - px;
    double ax = 1.0 - 2.0 * px, ay = 1.0 - 2.0 * py;
    double bx = 2.0 * px, by = 2.0 * py;
    for (var i = 0; i <= steps; i++) {
        double x = (double)i / steps;
        double r = (-bx + sqrt(bx * bx + 4.0 * ax * x)) / (2.0 * ax);
        double y = r * r * ay + by * r;
        var t1 = 0.0001 + 3.9999 * y;
        var t2 = 0.0001 + 7.9999 * y;
        Env::attackRates[i] = 1.0 / (*Elem::samplingRate * t1);
        Env::releaseRates[i] = 1.0 / (*Elem::samplingRate * t2);
        var n = t2 * *Elem::samplingRate;
        Env::releaseRatesExp[i] = pow(32767.0, -1.0 / n);
    }

    // dur = 1/100*samplingRate - 10ms
    // pow(factor, dur) = 1/32767
    Env::fadoutFactor = pow(2.0, -15.0 / (*Elem::samplingRate / 100.0));
}