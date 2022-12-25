#include "flt.h"
#include "math.h"

NS_FW_BASE_USE
using namespace SYNTH;

float Flt::cutoffTable[256];

Flt::Flt() {
    memset(&coeffs, 0, sizeof(FltCoeffs));
}
void Flt::assignControls(PotBase* controls) {
    controls_ = (FltCtrls*)controls;
    //controls_->amp.init(0.0f, 1.0f, 0.01f, 1.0f);
    controls_->cut.init(0, 255, 1, 0);
    controls_->res.init(0.0f, 1.0f, 0.01f, 0.0f);
    controls_->mod.init(0.0f, 1.0f, 0.01f, 0.5f);
    controls_->mode.init(0, FmAllPass, 1, FmLowPass);
}

void Flt::setFromStream(byte* stream) {
    //Elem::setFromStream(stream, (Pot*)controls_);
    controls_->cut.setFromStream(stream);
    controls_->res.setFromStream(stream);
    controls_->mod.setFromStream(stream);
    controls_->mode.setFromStream(stream);
}

void Flt::samplingRate(float* smpRate) {
    samplingRate_ = smpRate;
    //theta_ = (float)(2 * M_PI / *smpRate);
}

float Flt::run(Arg params) {
    var input = params.f;
    // Apply filter
    var gain = 1.0f / coeffs.ai[0];
    var lp = (coeffs.bi[0] * input + coeffs.bi[1] * coeffs.ui[0] + coeffs.bi[2] * coeffs.ui[1] - coeffs.ai[1] * coeffs.lp[0] - coeffs.ai[2] * coeffs.lp[1]) * gain;
    var hp = (coeffs.ci[0] * input + coeffs.ci[1] * coeffs.vi[0] + coeffs.ci[2] * coeffs.vi[1] - coeffs.ai[1] * coeffs.hp[0] - coeffs.ai[2] * coeffs.hp[1]) * gain;
    // lp inputs
    coeffs.ui[1] = coeffs.ui[0]; coeffs.ui[0] = input;
    // hp inputs
    coeffs.vi[1] = coeffs.vi[0]; coeffs.vi[0] = input;
    // lp outputs
    coeffs.lp[1] = coeffs.lp[0]; coeffs.lp[0] = lp;
    // hp outputs
    coeffs.hp[1] = coeffs.hp[0]; coeffs.hp[0] = hp;

    var output = 0.0;
    var mode = controls_->mode.value.b;
    if ((mode & FmLowPass) != 0) output += lp;
    if ((mode & FmHighPass) != 0) output += hp;
    if ((mode & FmBandPass) != 0) {
        output += input - hp - lp;
        gain *= 1.5;
    }
    return (float)(gain * output);
}

void Flt::update(float cut) {
    // Update filter coefficients
    var res = (controls_->res.value.f < 0.000001f) ? 1.0f : 1.0f - controls_->res.value.f;
    var e = 0.01f + 0.49f * (Flt::cutoffTable[controls_->cut.value.b] + controls_->mod.value.f * cut);
    var g = -res * e;
    var b0 = e * e;
    coeffs.bi[0] = coeffs.bi[1] = b0; coeffs.bi[2] = 2.0f * b0;
    coeffs.ci[0] = coeffs.ci[2] = 1; coeffs.ci[1] = -2.0f;
    coeffs.ai[0] = b0 + 1.0f - g;
    coeffs.ai[2] = b0 + 1.0f + g;
    coeffs.ai[1] = 2.0f * (b0 - 1);
}


void Flt::initialize(float smpRate) {
    // calculate cutoff
    createBezierTable(cutoffTable, 0.85f, 255, [](float y, int i) { return 0.005f + y * 0.995f; });
    //int steps = 255;
    //float px = 0.8f;
    //float py = 1.0f - px;
    //float ax = 1.0f - 2.0f * px, ay = 1.0f - 2.0f * py;
    //float bx = 2.0f * px, by = 2.0f * py;
    //for (auto i = 0; i <= steps; i++) {
    //    float x = (float)i / steps;
    //    float r = (-bx + (float)sqrt(bx * bx + 4.0f * ax * x)) / (2.0f * ax);
    //    float y = r * r * ay + by * r;
    //    cutoffTable[i] = 1.0f / (smpRate * (0.005f + 0.995f * y));
    //}
}