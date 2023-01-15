#include "flt.h"
#include "math.h"
#include "base/memory.h"

// Linear factors
#define LF2P 1.4142135623730950488016887242097
#define LF4P1 0.7653668647301795434569199680608
#define LF4P2 1.8477590650225735122563663787936

/**************************************************************
1 pole: s + 1
    stage1
        b0 = w      b1 = w
        a0 = w+1    a1 = w-1

2 pole: s² + √2s + 1
    stage1
        C = √2
        b0 = w²           b1 = 2w²        b2 = w²
        a0 = w²+w*C+1     a1 = 2(w²-1)     a2 = w²-w*C+1

 3 pole: (s + 1)(s² + s + 1)
    stage1
        b0 = w      b1 = w
        a0 = w+1    a1 = w-1
    stage2
        C = 1
        b0 = w²         b1 = 2w²        b2 = w²
        a0 = w²+C*w+1   a1 = 2(w²-1)    a2 = w²-C*w+1

 4 pole: (s² + s*sqrt(2-√2) + 1)(s² + s*sqrt(2+√2) + 1)
    stage1
        C = sqrt(2-√2)
        b0 = w²         b1 = 2w²        b2 = w²
        a0 = w²+w*C+1   a1 = 2(w²-1)    a2 = w²-w*C+1
    stage2
        C = sqrt(2+√2)
        b0 = w²         b1 = 2w²        b2 = w²
        a0 = w²+w*C+1   a1 = 2(w²-1)    a2 = w²-w*C+1
**************************************************************/

NS_FW_BASE_USE
using namespace SYNTH;

#pragma region FltStage
FltStage::FltStage() {
    memset(&ai_, 0, sizeof(float) * 19);
}
void FltStage1Pole::run(Arg params) {
    var gain = 1.0f / ai_[0];
    // y0 = (b0*u0 + b1*u1 - a1*y1)/a0
    var lp = (bi_[0] * ui_[0] + bi_[1] * ui_[1] - ai_[1] * lp_[0]) * gain;
    var hp = (ci_[0] * vi_[0] + ci_[1] * vi_[1] - ai_[1] * hp_[0]) * gain;
    ui_[1] = ui_[0];
    vi_[1] = vi_[0];
    lp_[0] = lp;
    hp_[0] = hp;
}
    
void FltStage1Pole::update(float e, float g) {
    bi_[0] = bi_[1] = e;
    ci_[0] = 1.0f;      ci_[1] = -1.0f;
    ai_[0] = e + 1;     ai_[1] = e - 1;
}

FltStage2Pole::FltStage2Pole(float f) {
    linearFactor_ = f;
}

void FltStage2Pole::run(Arg params) {
    var gain = 1.0f / ai_[0];
    // y0 = (b0*u0 + b1*u1 + b2*u2 - a1*y1 - a2*y2)/a0
    var lp = (bi_[0] * ui_[0] + bi_[1] * ui_[1] + bi_[2] * ui_[2] - ai_[1] * lp_[0] - ai_[2] * lp_[1]) * gain;
    var hp = (ci_[0] * vi_[0] + ci_[1] * vi_[1] + ci_[2] * vi_[2] - ai_[1] * hp_[0] - ai_[2] * hp_[1]) * gain;
    ui_[2] = ui_[1]; ui_[1] = ui_[0];
    vi_[2] = vi_[1]; vi_[1] = vi_[0];
    lp_[1] = lp_[0]; lp_[0] = lp;
    hp_[1] = hp_[0]; hp_[0] = hp;
}

void FltStage2Pole::update(float e, float g) {
    g *= linearFactor_;
    var b0 = e * e;
    bi_[0] = bi_[2] = b0; bi_[1] = 2.0f * b0;
    ci_[0] = ci_[2] = 1.0f; ci_[1] = -2.0f;
    ai_[0] = b0 + 1 - g;
    ai_[2] = b0 + 1 + g;
    ai_[1] = 2.0f * (b0 - 1);
}
#pragma endregion

#pragma region Filter
float Flt::cutoffTable[256];
float pole2Factors[] = { 1.4142135623730950488016887242097f };  // sqrt(2)
float pole3Factors[] = { 1.0f };
float pole4Factors[] = { 0.7653668647301795434569199680608f, 1.8477590650225735122563663787936f };  // sqrt(2 + sqrt(2)), sqrt(2 - sqrt(2))
float pole5Factors[] = { 1.6180339887498948482045868343656f, 0.61803398874989484820458683436564f };  // 0.5(1 + sqrt(5)), 0.5(sqrt(5) - 1)
float pole6Factors[] = { 0.5176380902050415246977976752481f, 1.4142135623730950488016887242097f, 1.9318516525781365734994863994578f }; // sqrt(2 - sqrt(3)), sqrt(2), sqrt(2 + sqrt(3))

float* Flt::linearFactors[] = { NULL, (float*)&pole2Factors, (float*)&pole3Factors, (float*)&pole4Factors, (float*)&pole5Factors, (float*)&pole6Factors};

Flt::Flt(int poleCount) {
    controls_ = NULL;
    createStages(poleCount);
}
Flt::~Flt() {
    for (var i = 0; i < stageCount_; i++) {
        DEL_(stages_[i]);
    }
}

void Flt::assignControls(PotBase* controls) {
    controls_ = (FltCtrls*)controls;
    controls_->cut.init(0, 255, 1, 0);
    controls_->res.init(0.0f, 1.0f, 0.01f, 0.0f);
    controls_->mod.init(0.0f, 1.0f, 0.01f, 0.5f);
    controls_->mode.init(0, FmAllPass, 1, FmLowPass);
}

void Flt::createStages(int poleCount) {
    for (var i = 0; i < stageCount_; i++) {
        DEL_(stages_[i]);
    }
    // create stages according to poles
    var si = 0;
    while (poleCount > 1) {
        // add 2-poles
        stages_[si] = NEW_(FltStage2Pole, Flt::linearFactors[poleCount - 1][si]);
        poleCount -= 2;
        si++;
    }
    if (poleCount == 1) {
        stages_[si++] = NEW_(FltStage1Pole);
    }
    stageCount_ = si;
}

void Flt::setFromStream(byte* stream) {
    controls_->cut.setFromStream(stream);
    controls_->res.setFromStream(stream);
    controls_->mod.setFromStream(stream);
    controls_->mode.setFromStream(stream);
}

float Flt::run(Arg params) {
    var input = params.f;
    var lp = input;
    var hp = input;

    for (var i = 0; i < stageCount_; i++) {
        stages_[i]->ui_[0] = lp;
        stages_[i]->vi_[0] = hp;
        stages_[i]->run(params);
        lp = stages_[i]->lp_[0];
        hp = stages_[i]->hp_[0];
    }

    var output = 0.0;
    var mode = controls_->mode.value.b;
    if ((mode & FmLowPass) != 0) output += lp;
    if ((mode & FmHighPass) != 0) output += hp;
    if ((mode & FmBandPass) != 0) {
        output += (input - hp - lp);
    }
    return (float)output;
}

void Flt::update(float cut) {
    var res = (controls_->res.value.f < 0.000001f) ? 1.0f : 1.0f - controls_->res.value.f;
    var e = 0.01f + 0.49f * (Flt::cutoffTable[controls_->cut.value.b] + cut);
    var g = -res * e;

    for (var i = 0; i < stageCount_; i++) {
        stages_[i]->update(e, g);
    }
}

void Flt::initialize(float smpRate) {
    // calculate cutoff
    createBezierTable(cutoffTable, 0.85f, 255, [](float y, int i) { return 0.005f + y * 0.995f; });
}
#pragma endregion