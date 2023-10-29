#include "math.h"
#include "utils/utils.h"
#include "synth/src/elem/osc.h"

NS_FW_BASE_USE
using namespace SYNTH;

Osc::Osc() : Elem(OscCtrlCount) {
    reset();
}

void Osc::reset() {
    timer = 0.0;
    smp = 0.0f;
}

// float params[] = { 0.0f, 1.0f, 0.0f };
// run(&params);
float Osc::run(Arg params) {
    var ctrls = (OscCtrls*)controls_;
    var fm = ((float*)params.p)[0];
    var pm = ((float*)params.p)[1];
    var pitch = note_->value.b + ctrls->tune.value.b;
    var fre = (1.0f + fm) * (ctrls->fre.value.f + Elem::p2f((float)pitch));
    var delta = fre / *Elem::samplingRate;
    if (delta >= 1.0) {
        delta = 0.99999999f;
    }
    var psw = pm + ctrls->psw.value.f + 0.0000001f;
    smp = 0.0;
    var wf = ctrls->wave.value.b;
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
        smp += (timer <= delta || timer >= 0.5 && timer < 0.5 + delta) ? smp = (float)Utils::randomSigned() : smp;
        wfc++;
    }
    if (wfc > 1) {
        smp /= wfc;
    }
    timer += delta;
    if (timer > 1.0) {
        timer -= 1.0;
    }

    return (float)(ctrls->amp.value.f * smp);
}

void Osc::assignControls(PotBase* controls) {
    controls_ = controls;
    var ctrls = (OscCtrls*)controls_;
    ctrls->fre.init(-10.0f, 10.0f, 0.1f, 0.0f);
    ctrls->note.init(0, 255, 1, 0);
    ctrls->tune.init(0, 255, 1, 0);
    ctrls->psw.init(0, 255, 1, 128);
    ctrls->wave.init(0, 255, 1, WfSaw);
}

void Osc::setFromStream(byte*& stream) {
    var ctrls = (OscCtrls*)controls_;
    ctrls->fre.setFromStream(stream);
    ctrls->note.setFromStream(stream);
    ctrls->tune.setFromStream(stream);
    ctrls->psw.setFromStream(stream);
    ctrls->wave.setFromStream(stream);
}

void Osc::setNoteControl(Pot* note) {
    this->note_ = note;
}
