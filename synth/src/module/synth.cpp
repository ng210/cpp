#include "base/memory.h"
#include "synth/src/module/synth.h"

NS_FW_BASE_USE
using namespace SYNTH;

Synth::Synth(float* pSmpRate, int count) {
    setupVoice = (PVOICEHANDLER)&Synth::setupVoiceHandler;
    renderVoice = (PVOICERENDERER)&Synth::renderVoiceHandler;
    freeVoice = (PVOICEHANDLER)&Synth::freeVoiceHandler;
    setNoteVoice = (PVOICESETNOTEHANDLER)&Synth::setNoteVoiceHandler;
    SynthBase::initialize(pSmpRate, count);
    pControls_ = (PotBase*)&controls_;
    program_ = -1;
    createOutputBuffers(1);
    isMono_ = true;
}

Synth::~Synth() {
}

// Voice handling
void Synth::setupVoiceHandler(Voice& v) {
    // Envelopes
    v.envelopes = NEWARR(Env, 4);
    v.envelopes[0].assignControls((PotBase*)&controls_.amEnv);
    v.envelopes[1].assignControls((PotBase*)&controls_.fmEnv);
    v.envelopes[2].assignControls((PotBase*)&controls_.pmEnv);
    v.envelopes[3].assignControls((PotBase*)&controls_.ftEnv);
    // LFOs
    v.lfos = NEWARR(Lfo, 2);
    v.lfos[0].assignControls((PotBase*)&controls_.lfo1);
    v.lfos[1].assignControls((PotBase*)&controls_.lfo2);
    // Oscillators
    v.oscillators = NEWARR(Osc, 2);
    v.oscillators[0].setNoteControl(&v.note);
    v.oscillators[1].setNoteControl(&v.note);
    v.oscillators[0].assignControls((PotBase*)&controls_.osc1);
    v.oscillators[1].assignControls((PotBase*)&controls_.osc2);
    // Filter
    v.filters = NEWARR(Flt, 1);
    v.filters[0].createStages(3);
    v.filters[0].assignControls((PotBase*)&controls_.flt1);
}
void Synth::renderVoiceHandler(Voice& v, float* buffer, int start, int end) {
    for (var i = start; i < end; i++) {
        // run LFOs
        var lfo = v.lfos[0].run();
        var amp = v.lfos[0].controls()->amp.value.f;
        var am = (lfo - amp) / 2.0f + 1.0f;
        var fm = v.lfos[1].run();

        // run envelopes
        am *= v.envelopes[0].run();
        fm += v.envelopes[1].run();
        var pm = v.envelopes[2].run();
        var cut = v.envelopes[3].run();

        // run oscillators
        float args[3] = { am, fm, pm };
        var smp = v.oscillators[0].run(args);
        smp += v.oscillators[1].run(args);

        // run filter
        v.filters[0].update(cut);
        buffer[i] += v.filters[0].run(smp);
    }
}
void Synth::freeVoiceHandler(Voice& v) {
    DELARR(v.envelopes);
    DELARR(v.lfos);
    DELARR(v.oscillators);
    DELARR(v.filters);
}
void Synth::setNoteVoiceHandler(Voice& v, byte note, byte velocity) {
    v.velocity.value = velocity / 255.0f;
    v.note.value = note;
    for (var i = 0; i < 2; i++) v.lfos[i].reset();
    for (var i = 0; i < 4; i++) v.envelopes[i].setGate(velocity);
}

//Module
void Synth::initialize(byte** pData) {

}
