#include "osc.h"
#include "math.h"
#include "utils/utils.h"

NS_FW_BASE_USE
using namespace SYNTH;

Osc::Osc() : Mdl() {
    reset();
}

void Osc::reset() {
    timer = 0.0;
    smp = 0.0f;
}

// float params[] = { 0.0f, 1.0f, 0.0f };
// run(&params);
float Osc::run(void* params) {
    var am = ((float*)params)[0];
    var fm = ((float*)params)[1];
    var pm = ((float*)params)[2];
    var pitch = note->value_.b + controls_->tune.value_.b;
    var delta = (controls_->fre.value_.f + fm + Mdl::p2f((float)pitch)) / *samplingRate_;
    if (delta >= 1.0) {
        delta = 0.99999999f;
    }
    var psw = pm + controls_->psw.value_.f;
    var smp = 0.0;
    var wf = controls_->wave.value_.b;
    var wfc = 0;
    if ((wf & WfSinus) != 0) {
        smp += sin(SYNTH_THETA * timer);
        wfc++;
    }
    if ((wf & WfTriangle) != 0) {
        var tmp = (timer <= psw) ? timer / psw : (1.0 - timer) / (1.0 - psw);
        smp += 2 * tmp - 1.0;
        wfc++;
    }
    if ((wf & WfSaw) != 0) {
        var tmp = (timer <= psw) ? timer / psw : 0.0;
        smp += 2 * tmp - 1.0;
        wfc++;
    }
    if ((wf & WfPulse) != 0) {
        smp += timer <= psw ? 1.0 : -1.0;
        wfc++;
    }
    if ((wf & WfNoise) != 0) {
        if (timer < delta || timer > 0.5 && timer < 0.5 + delta) {
            smp += (float)Utils::randomSigned();
        }
        wfc++;
    }
    if (wfc > 1) {
        smp /= wfc;
    }
    timer += delta;
    if (timer > 1.0) {
        timer -= 1.0;
    }
    return (float)(controls_->amp.value_.f * am * smp);
}

void Osc::assignControls(Pot* controls) {
    controls_ = (OscCtrls*)controls;
    controls_->amp.init(0.0f, 1.0f, 0.01f, 1.0f);
    controls_->fre.init(0.0f, 0.5f * *samplingRate_, 100.0f, 0.0f);
    controls_->note.init(0, 255, 1, 24);
    controls_->tune.init(0, 255, 1, 0);
    controls_->psw.init(0.0f, 1.0f, 0.01f, 0.5f);
    controls_->wave.init(0, 255, 1, WfSaw);
}

void Osc::setFromStream(byte* stream) {
    Mdl::setFromStream(stream, (Pot*)controls_);
    controls_->fre.setFromStream(stream);
    controls_->note.setFromStream(stream);
    controls_->tune.setFromStream(stream);
    controls_->psw.setFromStream(stream);
    controls_->wave.setFromStream(stream);
}

void Osc::setNoteControl(Pot* note) {
    this->note = note;
}