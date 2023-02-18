#include <math.h>
#include "synth/src/elem/env.h"

using namespace SYNTH;

double Env::attackRates[256];
double Env::releaseRates[256];
double Env::releaseRatesExp[256];

Env::Env() {
    velocity_ = 0.0;
    phase_ = EnvPhase::Idle;
    timer_ = 0.0;
    rate_ = 0.0;
    ticks_ = 0;
}

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
}
