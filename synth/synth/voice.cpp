#include "voice.h"
#include "base/memory.h"

NS_FW_BASE_USE
using namespace SYNTH;

Voice::Voice() {
    // create a synth with 2 oscillators, 2 LFOs, 4 envelopes and a filter
    envelopes = NEWARR(Env, 4);
    lfos = NEWARR(Lfo, 2);
    oscillators = NEWARR(Osc, 2);
    oscillators[0].setNoteControl(&note_);
    oscillators[1].setNoteControl(&note_);
    filters = NEWARR(Flt, 1);
}

Voice::~Voice() {
    freeResources();
}

void Voice::freeResources() {
    DELARR(envelopes);
    DELARR(lfos);
    DELARR(oscillators);
    DELARR(filters);
}

void Voice::setNote(byte note, byte velocity) {
    velocity_.value_ = velocity/255.0f;
    note_.value_ = note;
    for (var i=0; i<2; i++) lfos[i].reset();
    for (var i=0; i<4; i++) envelopes[i].setGate(velocity);
}

void Voice::setSamplingRate(float* samplingRate) {
    lfos[0].samplingRate(samplingRate);
    lfos[1].samplingRate(samplingRate);
    envelopes[0].samplingRate(samplingRate);
    envelopes[1].samplingRate(samplingRate);
    envelopes[2].samplingRate(samplingRate);
    envelopes[3].samplingRate(samplingRate);
    oscillators[0].samplingRate(samplingRate);
    oscillators[1].samplingRate(samplingRate);
    filters[0].samplingRate(samplingRate);
}

float Voice::run() {
    // run LFOs
    // var am = lfos[0].run();
    var amp =lfos[0].controls()->amp.value_.f;
    //var am = (lfos[0].run() + amp)/2.0f + (1.0f-amp);
    var am = (lfos[0].run() - amp)/2.0f + 1.0f;
    var fm = lfos[1].run();

    // run envelopes
    var c1 = 1.0f;
        am = envelopes[0].run(&am);
    var pm = envelopes[1].run(&c1);
    var cut = envelopes[2].run(&c1);
        fm += envelopes[3].run(&c1);

    // run oscillators
    float args[3] = { am, fm, pm };
    var smp  = oscillators[0].run(args);
        smp += oscillators[1].run(args);
        filters[0].update(cut);
    return filters[0].run(&smp);
}
