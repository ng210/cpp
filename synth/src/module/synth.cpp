#include "base/memory.h"
#include "synth/src/module/synth.h"

NS_FW_BASE_USE
using namespace SYNTH;

Synth::Synth(int count) : SynthBase() {
    setupVoiceHandler = &Synth::setupVoiceHandler_;
    renderVoiceHandler = &Synth::renderVoiceHandler_;
    //freeVoice = (LpVoiceHandler)&Synth::freeVoiceHandler;
    //setNoteVoice = (LpVoiceSetNoteHandler)&Synth::setNoteVoiceHandler;
    SynthBase::initialize(count);
    createOutputBuffers(1);
}

Synth::~Synth() {
}

// Voice handling
void Synth::setupVoiceHandler_(Voice* v, Value* values) {
    var synthValues = (SynthValues*)values;
    v->setupVoiceAdsr(4, &synthValues->amAdsr);
    v->setupVoiceLfo(2, &synthValues->lfo1);
    v->setupVoiceOsc(2, &synthValues->osc1);
    v->setupVoiceFlt(1, &synthValues->flt1);
}
void Synth::renderVoiceHandler_(Voice* v, SynthBase* synth, float* buffer, int start, int end) {
    for (var i = start; i < end; i++) {
        // run LFOs
        var lfoAm = 0.5f * (v->lfos[0].run() + 1.0f);
        var lfoFm = v->lfos[1].run();

        // run envelopes
        var envAm = ((Adsr*)v->envelopes)[0].run();
        var envFm = ((Adsr*)v->envelopes)[1].run();
        var envPm = ((Adsr*)v->envelopes)[2].run();
        var envCm = ((Adsr*)v->envelopes)[3].run();

        var fm = envFm + lfoFm;

        // run oscillators
        float args[2] = { fm, envPm };
        var smp = v->oscillators[0].run(args);
        smp += v->oscillators[1].run(args);

        // run filter
        v->filters[0].update(envCm);
        var output = v->filters[0].run(smp);
        buffer[i] += envAm * lfoAm * output;
    }
}
//void Synth::freeVoiceHandler(Voice& v) {
//    DELARR(v.envelopes);
//    DELARR(v.lfos);
//    DELARR(v.oscillators);
//    DELARR(v.filters);
//}
//void Synth::setNoteVoiceHandler(Voice& v, byte note, byte velocity) {
//    v.velocity = velocity / 255.0f;
//    v.note = note;
//    if (velocity != 0) {
//        for (var i = 0; i < 2; i++) v.lfos[i].reset();
//    }
//    for (var ei = 0; ei < 4; ei++) ((Adsr*)v.envelopes)[ei].setGate(velocity);
//}

Value* Synth::getValues() {
    return (Value*)&values_;
}