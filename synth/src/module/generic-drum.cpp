#include "math.h"
#include "base/memory.h"
#include "utils/utils.h"
#include "synth/src/module/generic-drum.h"
#include "synth/src/module/synth-def.h"

#include "base/debug.h"

NS_FW_BASE_USE
using namespace SYNTH;

#define SMPSINUS(timer) sin(SYNTH_THETA * timer)
#define SMPPULSE(timer) (timer <= 0.5 ? 1.0 : -1.0)
#define SMPNOISE(timer, delta, store) (timer < delta ? store = (float)Utils::randomSigned() : store)

// Voice handling
void GenericDrum::setupVoiceHandler_(Voice* v, Value* values) {
    var gdValues = (GenericDrumValues*)values;
    v->setupVoiceDahr(4, &gdValues->dahr[0]);
    v->setupVoiceOsc(6, &gdValues->osc[0]);
    v->setupVoiceFlt(2, &gdValues->flt[0]);
}

void GenericDrum::renderVoiceHandler_(Voice* v, SynthBase* synth, float* buffer, int start, int end) {
    ((GenericDrum*)synth)->render_(buffer, v, start, end);
}

GenericDrum::GenericDrum() : SynthBase() {
    setupVoiceHandler = GenericDrum::setupVoiceHandler_;
    renderVoiceHandler = GenericDrum::renderVoiceHandler_;
    //freeVoice = (LpVoiceHandler)&GenericDrum::freeVoiceHandler;
    //setNoteVoice = (LpVoiceSetNoteHandler)&GenericDrum::setNoteVoiceHandler;
    //setType.add(this, GenericDrum::typeSetter);
    render_ = NULL;
    voiceCount(2);
    GenericDrum::typeSetter(this, values_.type.b);
    //noise = 0.0f;
    //note_ = 0;
    //baseFreq_ = 0.0f;
    //memset(timers, 0, sizeof(float) * 6);
    createOutputBuffers(1);
}

GenericDrum::~GenericDrum() {
}

Value* GenericDrum::getValues() {
    return (Value*)&values_;
}

//bool GenericDrum::isActive() {
//    return envelopes[0].isActive();
//}

float GenericDrum::runFilter(Flt* flt, float cut, float input) {
    var st = flt->stage(0);
    var q = flt->values()->res.f;
    var f = 0.5f * (flt->values()->cut.b / 255.0f + cut);   // Flt::cutoffTable[flt->values()->cut.b] + cut;
    var fb = q + q / (1.0f - f);
    st->ai_[0] = st->ai_[0] + f * (input - st->ai_[0] + fb * (st->ai_[0] - st->ai_[1]));
    st->ai_[1] = st->ai_[1] + f * (st->ai_[0] - st->ai_[1]);
    return (float)st->ai_[1];
}

void GenericDrum::renderDefault(float* buffer, Voice* v, int start, int end) {
    for (var i = start; i < end; i++) {
        // run envelopes
        var amp = ((GenericDrumValues*)v->values)->dahr[0].amp.f;
        float am = ((Dahr*)v->envelopes)[0].run();
        float fm = ((Dahr*)v->envelopes)[1].run();
        float cl = ((Dahr*)v->envelopes)[2].run();
        float cm = ((Dahr*)v->envelopes)[3].run();

        // run oscillators
        float args[2] = { fm, 0.1f };
        var osc1 = v->oscillators[0].run(args);
        var osc2 = v->oscillators[1].run(args);
        args[0] += cl;
        var click = amp * cl * v->oscillators[2].run(args);
        var noise = am * v->oscillators[3].run(args);

        // run filters
        v->filters[0].update(cm);
        v->filters[1].update(cm);
        var flt1 = v->filters[0].run(noise);
        buffer[i] += am * (osc1 + osc2) + v->filters[1].run(flt1 + click);
    }
}

void GenericDrum::renderHihat(float* buffer, Voice* v, int start, int end) {
    for (var i = start; i < end; i++) {
        // run envelopes
        float am = ((Dahr*)v->envelopes)[0].run();
        float fm = ((Dahr*)v->envelopes)[1].run();
        float cl = ((Dahr*)v->envelopes)[2].run();
        float cm = ((Dahr*)v->envelopes)[3].run();

        var amp = am * (1.0f + cl);

        // run oscillators
        float args[2] = { fm, 0.0f };
        float smp = 0.0f;
        for (var oi = 0; oi < 6; oi++) {
            smp += v->oscillators[oi].run(args);
        }
        // run filters
        v->filters[0].update(cm);
        v->filters[1].update(0.5f * (cm + cl));
        var flt1 = v->filters[0].run(smp);
        buffer[i] += amp * v->filters[1].run(flt1);
    }
}

void GenericDrum::renderClap(float* buffer, Voice* v, int start, int end) {
    for (var i = start; i < end; i++) {
        // run envelopes
        var am = 0.0f;
        for (var ei = 0; ei < 4; ei++) {
            am += v->envelopes[ei].run();
        }

        // run oscillators
        float args[2] = { 0.0f, 0.0f };
        float smp = 0.0f;
        for (var oi = 0; oi < 3; oi++) {
            smp += v->oscillators[oi].run(args);
        }

        // run filters
        v->filters[0].update(0.0f);
        v->filters[1].update(am);
        var hp = v->filters[0].run((float)smp);
        var lp = v->filters[1].run(hp);
        buffer[i] += am * lp;
    }
}

int GenericDrum::typeSetter(void* obj, Value value, void* unused) {
    var drum = (GenericDrum*)obj;
    drum->render_ = &GenericDrum::renderDefault;
    switch (value.b) {
        case HihatType:
            drum->render_ = &GenericDrum::renderHihat;
            for (var oi = 0; oi < 4; oi++) {
                drum->values_.osc[oi].wave = OscWaveform::WfPulse;
                drum->values_.osc[oi].psw.f = 0.5f;
            }

            for (var vi = 0; vi < drum->voiceCount_; vi++) {
                var vu = &drum->voiceUnits_[vi];
                for (var vj = 0; vj < 2; vj++) {
                    vu->voices()[vj]->filters[0].createStages(6);
                    vu->voices()[vj]->filters[1].createStages(8);
                }
            }
            break;
        //case ClapType:
        //    drum->render_ = &GenericDrum::renderClap;
        //    // break???
        case BassDrumType:
        default:
            //drum->oscCount_ = 4;
            drum->values_.osc[0].wave = OscWaveform::WfSinus;
            drum->values_.osc[1].wave = OscWaveform::WfSinus;
            drum->values_.osc[2].wave = OscWaveform::WfTriangle;
            drum->values_.osc[2].psw.f = 0.4f;
            drum->values_.osc[3].wave = OscWaveform::WfNoise;
            for (var vi = 0; vi < drum->voiceCount_; vi++) {
                var vu = &drum->voiceUnits_[vi];
                for (var vj = 0; vj < 2; vj++) {
                    vu->voices()[vj]->filters[0].createStages(2);
                    vu->voices()[vj]->filters[1].createStages(2);
                }
            }
            break;
    }
    return 1;
}
