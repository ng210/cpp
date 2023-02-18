#include "math.h"
#include "utils/utils.h"
#include "synth/src/elem/osc.h"

NS_FW_BASE_USE
using namespace SYNTH;

Osc::Osc() : Elem() {
    reset();
}

void Osc::reset() {
    timer = 0.0;
    smp_ = 0.0f;
}

// float params[] = { 0.0f, 1.0f, 0.0f };
// run(&params);
float Osc::run(Arg params) {
    var am = ((float*)params.p)[0];
    var fm = ((float*)params.p)[1];
    var pm = ((float*)params.p)[2];
    var pitch = note_->value.b + controls_->tune.value.b;
    var delta = (controls_->fre.value.f + fm + Elem::p2f((float)pitch)) / *Elem::samplingRate;
    if (delta >= 1.0) {
        delta = 0.99999999f;
    }
    var psw = pm + controls_->psw.value.f + 0.0000001f;
    var smp = 0.0;
    var wf = controls_->wave.value.b;
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
        smp += (timer <= delta || timer >= 0.5 && timer < 0.5 + delta) ? smp_ = (float)Utils::randomSigned() : smp_;
        wfc++;
    }
    if (wfc > 1) {
        smp /= wfc;
    }
    timer += delta;
    if (timer > 1.0) {
        timer -= 1.0;
    }
    return (float)(controls_->amp.value.f * am * smp);
}

void Osc::assignControls(PotBase* controls) {
    controls_ = (OscCtrls*)controls;
    controls_->amp.init(0, 255, 1, 128);
    controls_->fre.init(-10.0f, 10.0f, 0.1f, 0.0f);
    controls_->note.init(0, 255, 1, 0);
    controls_->tune.init(0, 255, 1, 0);
    controls_->psw.init(0, 255, 1, 128);
    controls_->wave.init(0, 255, 1, WfSaw);
}

void Osc::setFromStream(byte*& stream) {
    controls_->amp.setFromStream(stream);
    controls_->fre.setFromStream(stream);
    controls_->note.setFromStream(stream);
    controls_->tune.setFromStream(stream);
    controls_->psw.setFromStream(stream);
    controls_->wave.setFromStream(stream);
}

void Osc::setNoteControl(Pot* note) {
    this->note_ = note;
}
