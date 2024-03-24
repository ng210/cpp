#include "base/memory.h"
#include "synth/src/module/bass.h"

NS_FW_BASE_USE
using namespace SYNTH;

Bass::Bass(int count) : SynthBase() {
    setupVoiceHandler = &Bass::setupVoiceHandler_;
    renderVoiceHandler = &Bass::renderVoiceHandler_;
    //freeVoice = (LpVoiceHandler)&Bass::freeVoiceHandler;
    //setNoteVoice = (LpVoiceSetNoteHandler)&Bass::setNoteVoiceHandler;
    SynthBase::initialize(count);
    createOutputBuffers(1);
}

Bass::~Bass() {
}

// Voice handling
void Bass::setupVoiceHandler_(Voice* v, Value* values) {
    var bassValues = (BassValues*)values;
    v->Voice::setupVoiceAdsr(3, &bassValues->amAdsr);
    v->Voice::setupVoiceOsc(2, &bassValues->osc1);
    v->Voice::setupVoiceFlt(1, &bassValues->flt1);
}
void Bass::renderVoiceHandler_(Voice* v, SynthBase* synth, float* buffer, int start, int end) {
    for (var i = start; i < end; i++) {
        // run envelopes
        var am = ((Adsr*)v->envelopes)[0].run();
        var pm = ((Adsr*)v->envelopes)[1].run();
        var cut = ((Adsr*)v->envelopes)[2].run();

        // run oscillators
        float args[2] = { 1.0f, pm };
        var smp = v->oscillators[0].run(args);
        smp += v->oscillators[1].run(args);

        // run filter
        v->filters[0].update(cut);
        buffer[i] += am * v->filters[0].run(smp);
    }
}

Value* Bass::getValues() {
    return (Value*)&values_;
}