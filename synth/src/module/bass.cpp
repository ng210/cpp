#include "base/memory.h"
#include "synth/src/module/bass.h"

NS_FW_BASE_USE
using namespace SYNTH;

Soundbank* Bass::defaultSoundbank_ = NULL;

Bass::Bass(float* smpRate, int count) : SynthBase((PotBase*)&controls, BassCtrlCount) {
    setupVoice = (PVOICEHANDLER)&Bass::setupVoiceHandler;
    renderVoice = (PVOICERENDERER)&Bass::renderVoiceHandler;
    freeVoice = (PVOICEHANDLER)&Bass::freeVoiceHandler;
    setNoteVoice = (PVOICESETNOTEHANDLER)&Bass::setNoteVoiceHandler;
    SynthBase::initialize(smpRate, count);
    program_ = 0;
    createOutputBuffers(1);
    setSoundbank(getDefaultSoundbank());
}

Bass::~Bass() {
}

// Voice handling
void Bass::setupVoiceHandler(Voice& v) {
    // Envelopes
    v.envelopes = NEWARR(Adsr, 3);
    v.envelopes[0].assignControls((PotBase*)&controls.amAdsr);
    v.envelopes[1].assignControls((PotBase*)&controls.pmAdsr);
    v.envelopes[2].assignControls((PotBase*)&controls.ftAdsr);
    // Oscillators
    v.oscillators = NEWARR(Osc, 2);
    v.oscillators[0].setNoteControl(&v.note);
    v.oscillators[1].setNoteControl(&v.note);
    v.oscillators[0].assignControls((PotBase*)&controls.osc1);
    v.oscillators[1].assignControls((PotBase*)&controls.osc2);
    // Filter
    v.filters = NEWARR(Flt, 1);
    v.filters[0].createStages(3);
    v.filters[0].assignControls((PotBase*)&controls.flt1);
}
void Bass::renderVoiceHandler(Voice& v, float* buffer, int start, int end) {
    for (var i = start; i < end; i++) {
        // run envelopes
        var am = v.envelopes[0].run();
        var pm = v.envelopes[1].run();
        var cut = v.envelopes[2].run();

        // run oscillators
        float args[3] = { am, 0.0f, pm };
        var smp = v.oscillators[0].run(args);
        smp += v.oscillators[1].run(args);

        // run filter
        v.filters[0].update(cut);
        buffer[i] += v.filters[0].run(smp);
    }
}
void Bass::freeVoiceHandler(Voice& v) {
    DELARR(v.envelopes);
    DELARR(v.oscillators);
    DELARR(v.filters);
}
void Bass::setNoteVoiceHandler(Voice& v, byte note, byte velocity) {
    v.velocity.value = velocity / 255.0f;
    v.note.value = note;
    for (var i = 0; i < 3; i++) v.envelopes[i].setGate(velocity);
}

//Module
Soundbank* Bass::getDefaultSoundbank() {
    return Bass::defaultSoundbank_;
}

void Bass::prepare() {
    var bass1 = NEW_(Stream);
    {
        //amAdsrAmp, amAdsrDc, amAdsrAtk, amAdsrDec, amAdsrSus, amAdsrRel
        bass1->writeFloat(1.0f)->writeFloat(0.0f)->writeByte(2)->writeByte(28)->writeByte(180)->writeByte(12);
        //pmAdsrAmp, pmAdsrDc, pmAdsrAtk, pmAdsrDec, pmAdsrSus, pmAdsrRel
        bass1->writeFloat(0.2f)->writeFloat(0.0f)->writeByte(10)->writeByte(44)->writeByte(128)->writeByte(200);
        //ftAdsrAmp, ftAdsrDc, ftAdsrAtk, ftAdsrDec, ftAdsrSus, ftAdsrRel
        bass1->writeFloat(0.5f)->writeFloat(0.0f)->writeByte(2)->writeByte(22)->writeByte(40)->writeByte(122);

        //osc1Amp, osc1Fre, osc1Note, osc1Tune, osc1Psw, osc1Wave
        bass1->writeByte(160)->writeFloat(0.0f)->writeByte(0)->writeByte(0)->writeByte(120)->writeByte(WfPulse);
        //osc2Amp, osc2Fre, osc2Note, osc2Tune, osc2Psw, osc2Wave
        bass1->writeByte(120)->writeFloat(0.2f)->writeByte(0)->writeByte(12)->writeByte(130)->writeByte(WfPulse);

        //flt1Cut, flt1Res, flt1Mod, flt1Mode
        bass1->writeByte(10)->writeByte(120)->writeByte(0)->writeByte(FmLowPass);
    }

    Bass bass(NULL, 1);
    Bass::defaultSoundbank_ = bass.createSoundbank();

    Bass::defaultSoundbank_->add("bass1..........", bass1->data());
    DEL_(bass1);
}

void Bass::cleanUp() {
    DEL_(Bass::defaultSoundbank_);
    Bass::defaultSoundbank_ = NULL;
}