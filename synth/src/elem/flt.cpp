#include "flt.h"
#include "math.h"
#include "base/memory.h"

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
    memset(&ai_, 0, sizeof(double) * 19);
}
void FltStage1Pole::run(Arg params) {
    //var gain = 1.0f / ai_[0];
    // y0 = (b0*u0 + b1*u1 - a1*y1)/a0
    var lp = (bi_[0] * ui_[0] + bi_[1] * ui_[1] - ai_[1] * lp_[0]) / ai_[0];        // * gain;
    var hp = (ci_[0] * vi_[0] + ci_[1] * vi_[1] - ai_[1] * hp_[0]) / ai_[0];
    ui_[1] = ui_[0];
    vi_[1] = vi_[0];
    lp_[0] = lp;
    hp_[0] = hp;
}
    
void FltStage1Pole::update(double e, double g) {
    bi_[0] = bi_[1] = e;
    ci_[0] = 1.0f;      ci_[1] = -1.0f;
    ai_[0] = e + 1;     ai_[1] = e - 1;
}

FltStage2Pole::FltStage2Pole(double f) {
    linearFactor_ = f;
}

void FltStage2Pole::run(Arg params) {
    //var gain = 1.0f / ai_[0];
    // y0 = (b0*u0 + b1*u1 + b2*u2 - a1*y1 - a2*y2)/a0
    var lp = (bi_[0] * ui_[0] + bi_[1] * ui_[1] + bi_[2] * ui_[2] - ai_[1] * lp_[0] - ai_[2] * lp_[1]) / ai_[0];    // * gain;
    var hp = (ci_[0] * vi_[0] + ci_[1] * vi_[1] + ci_[2] * vi_[2] - ai_[1] * hp_[0] - ai_[2] * hp_[1]) / ai_[0];
    ui_[2] = ui_[1]; ui_[1] = ui_[0];
    vi_[2] = vi_[1]; vi_[1] = vi_[0];
    lp_[1] = lp_[0]; lp_[0] = lp;
    hp_[1] = hp_[0]; hp_[0] = hp;
}

void FltStage2Pole::update(double e, double g) {
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
double Flt::cutoffTable[256];
double pole2Factors[] = { M_SQRT2 };  // sqrt(2)
double pole3Factors[] = { 1.0 };
double pole4Factors[] = { 0.7653668647301795434569199680608, 1.8477590650225735122563663787936 };  // sqrt(2 + sqrt(2)), sqrt(2 - sqrt(2))
double pole5Factors[] = { 0.61803398874989484820458683436564, 1.6180339887498948482045868343656 };  // 0.5(1 + sqrt(5)), 0.5(sqrt(5) - 1)
double pole6Factors[] = { 0.5176380902050415246977976752481, M_SQRT2, 1.9318516525781365734994863994578 }; // sqrt(2 - sqrt(3)), sqrt(2), sqrt(2 + sqrt(3))
double pole7Factors[] = { 0.445042, 1.24698, 1.801938 };
double pole8Factors[] = { 0.390181, 1.11114, 1.662939, 1.961571 };
double pole9Factors[] = { 0.347296, 1.0, 1.532089, 1.879385 };
double pole10Factors[] = { 0.312869, 0.907981, M_SQRT2, 1.782013, 1.975377 };

double* Flt::linearFactors[] = { NULL, (double*)&pole2Factors, (double*)&pole3Factors, (double*)&pole4Factors, (double*)&pole5Factors, (double*)&pole6Factors, (double*)&pole7Factors, (double*)&pole8Factors, (double*)&pole9Factors, (double*)&pole10Factors};

Flt::Flt(int poleCount) :Elem(FltCtrlCount) {
    controls_ = NULL;
    poleCount_ = poleCount;
    createStages(poleCount);
}
Flt::~Flt() {
    for (var i = 0; i < stageCount_; i++) {
        DEL_(stages_[i]);
    }
}

void Flt::assignControls(PotBase* controls) {
    controls_ = controls;
    var ctrls = (FltCtrls*)controls_;
    ctrls->cut.init(0, 255, 1, 0);
    ctrls->res.init(0, 255, 1, 0);
    ctrls->mod.init(0.0f, 1.0f, 0.01f, 0.5f);
    ctrls->mode.init(0, FmAllPass, 1, FmLowPass);
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

void Flt::setFromStream(byte*& stream) {
    var ctrls = (FltCtrls*)controls_;
    ctrls->cut.setFromStream(stream);
    ctrls->res.setFromStream(stream);
    ctrls->mod.setFromStream(stream);
    ctrls->mode.setFromStream(stream);
}

float Flt::run(Arg params) {
    var input = params.f;
    var lp = (double)input;
    var hp = lp;

    for (var i = 0; i < stageCount_; i++) {
        stages_[i]->ui_[0] = lp;
        stages_[i]->vi_[0] = hp;
        stages_[i]->run(params);
        lp = stages_[i]->lp_[0];
        hp = stages_[i]->hp_[0];
    }

    //hp = input - lp;

    var output = 0.0;
    var ctrls = (FltCtrls*)controls_;
    var mode = ctrls->mode.value.b;
    if ((mode & FmLowPass) != 0) output += lp;
    if ((mode & FmHighPass) != 0) output += hp;
    if ((mode & FmBandPass) != 0) {
        output += (input - hp - lp);
    }
    return (float)output;
}

void Flt::update(float cut) {
    var ctrls = (FltCtrls*)controls_;
    var res = (ctrls->res.value.f < 0.000001f) ? 1.0f : 1.0f - ctrls->res.value.f;
    var e = 0.5f * (Flt::cutoffTable[ctrls->cut.value.b] + cut);
    if (e <= 0) e = 0.001f;
    var g = -res * e;   // (float)-pow(res, 0.5f / poleCount_) * e;

    for (var i = 0; i < stageCount_; i++) {
        stages_[i]->update(e, g);
    }
}

void Flt::initialize() {
    // calculate cutoff
    createBezierTable(cutoffTable, 0.85f, 255, [](double y, int i) { return M_PI * (0.0002 + y * 0.9998); });
}

#pragma endregion