#include "base/memory.h"
#include "synth/src/module/bass.h"

NS_FW_BASE_USE
using namespace SYNTH;

Bass::Bass(int count) : SynthBase() {
    setupVoice = (LpVoiceHandler)&Bass::setupVoiceHandler;
    renderVoice = (LpVoiceRenderer)&Bass::renderVoiceHandler;
    freeVoice = (LpVoiceHandler)&Bass::freeVoiceHandler;
    setNoteVoice = (LpVoiceSetNoteHandler)&Bass::setNoteVoiceHandler;
    SynthBase::initialize(count);
    createOutputBuffers(1);
}

Bass::~Bass() {
}

// Voice handling
void Bass::setupVoiceHandler(Voice& v) {
    // Envelopes
    v.envelopes = NEWARR(Adsr, 3);
    v.envelopes[0].values(&values_.amAdsr);
    v.envelopes[1].values(&values_.pmAdsr);
    v.envelopes[2].values(&values_.ftAdsr);
    // Oscillators
    v.oscillators = NEWARR(Osc, 2);
    v.oscillators[0].setNoteControl(&v.note);
    v.oscillators[1].setNoteControl(&v.note);
    v.oscillators[0].values(&values_.osc1);
    v.oscillators[1].values(&values_.osc2);
    // Filter
    v.filters = NEWARR(Flt, 1);
    v.filters[0].createStages(3);
    v.filters[0].values(&values_.flt1);
}

void Bass::renderVoiceHandler(Voice& v, float* buffer, int start, int end) {
    for (var i = start; i < end; i++) {
        // run envelopes
        var am = v.envelopes[0].run();
        var pm = v.envelopes[1].run();
        var cut = v.envelopes[2].run();

        // run oscillators
        float args[2] = { 1.0f, pm };
        var smp = v.oscillators[0].run(args);
        smp += v.oscillators[1].run(args);

        // run filter
        v.filters[0].update(cut);
        buffer[i] += v.filters[0].run(am * smp);
    }
}
void Bass::freeVoiceHandler(Voice& v) {
    DELARR(v.envelopes);
    DELARR(v.oscillators);
    DELARR(v.filters);
}
void Bass::setNoteVoiceHandler(Voice& v, byte note, byte velocity) {
    v.velocity = velocity / 255.0f;
    v.note = note;
    for (var i = 0; i < 3; i++) v.envelopes[i].setGate(velocity);
}

Value* Bass::getValues() {
    return (Value*)&values_;
}