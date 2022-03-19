#include "flt.h"
#include "math.h"

NS_FW_BASE_USE
using namespace SYNTH;

Flt::Flt() {
    memset(&coeffs, 0, sizeof(FltCoeffs));
}
void Flt::assignControls(Pot* controls) {
    controls_ = (FltCtrls*)controls;
    controls_->amp.init(0.0f, 1.0f, 0.01f, 1.0f);
    controls_->cut.init(0.0f, 0.1f, 0.1f, 0.5f);
    controls_->res.init(0.0f, 1.0f, 0.01f, 0.0f);
    controls_->mod.init(0.0f, 1.0f, 0.01f, 0.5f);
    controls_->mode.init(0, FmAllPass, 1, FmLowPass);
}

void Flt::setFromStream(byte* stream) {
    Mdl::setFromStream(stream, (Pot*)controls_);
    controls_->cut.setFromStream(stream);
    controls_->res.setFromStream(stream);
    controls_->mod.setFromStream(stream);
    controls_->mode.setFromStream(stream);
}

void Flt::samplingRate(float* smpRate) {
    samplingRate_ = smpRate;
    theta_ = (float)(2 * M_PI / *smpRate);
}

float Flt::run(void* params) {
    var input = *(float*)params;
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
    var mode = controls_->mode.value_.b;
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
    var res = (controls_->res.value_.f < 0.000001f) ? 1.0f : 1.0f - controls_->res.value_.f;
    var e = (controls_->cut.value_.f + controls_->mod.value_.f * cut)/2.0f;
    //var e = (float)tan(cut * theta_);
    var g = -res * e;
    var b0 = e * e;
    coeffs.bi[0] = coeffs.bi[1] = b0; coeffs.bi[2] = 2.0f * b0;
    coeffs.ci[0] = coeffs.ci[2] = 1; coeffs.ci[1] = -2.0f;
    coeffs.ai[0] = b0 + 1.0f - g;
    coeffs.ai[2] = b0 + 1.0f + g;
    coeffs.ai[1] = 2.0f * (b0 - 1);
}
