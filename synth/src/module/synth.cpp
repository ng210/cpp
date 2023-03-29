#include "base/memory.h"
#include "synth/src/module/synth.h"

NS_FW_BASE_USE
using namespace SYNTH;

Soundbank* Synth::defaultSoundbank_;

Synth::Synth(float* pSmpRate, int count) : SynthBase((PotBase*)&controls, SynthCtrlCount) {
    setupVoice = (PVOICEHANDLER)&Synth::setupVoiceHandler;
    renderVoice = (PVOICERENDERER)&Synth::renderVoiceHandler;
    freeVoice = (PVOICEHANDLER)&Synth::freeVoiceHandler;
    setNoteVoice = (PVOICESETNOTEHANDLER)&Synth::setNoteVoiceHandler;
    SynthBase::initialize(pSmpRate, count);
    program_ = -1;
    createOutputBuffers(1);
    setSoundbank(getDefaultSoundbank());
}

Synth::~Synth() {
}

// Voice handling
void Synth::setupVoiceHandler(Voice& v) {
    // Envelopes
    v.envelopes = NEWARR(Adsr, 4);
    v.envelopes[0].assignControls((PotBase*)&controls.amAdsr);
    v.envelopes[1].assignControls((PotBase*)&controls.fmAdsr);
    controls.fmAdsr.amp.init(0.0f, 220.0f, 0.1f, 0.0f);
    v.envelopes[2].assignControls((PotBase*)&controls.pmAdsr);
    v.envelopes[3].assignControls((PotBase*)&controls.ftAdsr);
    // LFOs
    v.lfos = NEWARR(Lfo, 2);
    v.lfos[0].assignControls((PotBase*)&controls.lfo1);
    v.lfos[1].assignControls((PotBase*)&controls.lfo2);
    controls.lfo2.amp.init(0.0f, 20.0f, 0.1f, 0.0f);
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
        float args[3] = { 1.0f, fm, pm };
        var smp = v.oscillators[0].run(args);
        smp += v.oscillators[1].run(args);

        // run filter
        v.filters[0].update(cut);
        buffer[i] += am * v.filters[0].run(smp);
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

Soundbank* Synth::getDefaultSoundbank() {
    return Synth::defaultSoundbank_;
}

//Module
void Synth::prepare() {
    var bass = NEW_(Stream);
    {
        //amAdsrAmp, amAdsrDc, amAdsrAtk, amAdsrDec, amAdsrSus, amAdsrRel
        bass->writeFloat(0.6f)->writeFloat(0.0f)->writeByte(2)->writeByte(28)->writeByte(180)->writeByte(12);
        //fmAdsrAmp, fmAdsrDc, fmAdsrAtk, fmAdsrDec, fmAdsrSus, fmAdsrRel
        bass->writeFloat(0.0f)->writeFloat(0.0f)->writeByte(0)->writeByte(0)->writeByte(0)->writeByte(0);
        //pmAdsrAmp, pmAdsrDc, pmAdsrAtk, pmAdsrDec, pmAdsrSus, pmAdsrRel
        bass->writeFloat(0.2f)->writeFloat(0.0f)->writeByte(10)->writeByte(44)->writeByte(128)->writeByte(200);
        //ftAdsrAmp, ftAdsrDc, ftAdsrAtk, ftAdsrDec, ftAdsrSus, ftAdsrRel
        bass->writeFloat(0.5f)->writeFloat(0.0f)->writeByte(2)->writeByte(22)->writeByte(40)->writeByte(122);

        //amLfoAmp, amLfoFre
        bass->writeFloat(0.2f)->writeFloat(2.0f);
        //fmLfoAmp, fmLfoFre
        bass->writeFloat(1.2f)->writeFloat(3.8f);

        //osc1Amp, osc1Fre, osc1Note, osc1Tune, osc1Psw, osc1Wave
        bass->writeByte(160)->writeFloat(0.0f)->writeByte(0)->writeByte(0)->writeByte(120)->writeByte(WfPulse);
        //osc2Amp, osc2Fre, osc2Note, osc2Tune, osc2Psw, osc2Wave
        bass->writeByte(120)->writeFloat(0.2f)->writeByte(0)->writeByte(12)->writeByte(130)->writeByte(WfPulse);

        //flt1Cut, flt1Res, flt1Mod, flt1Mode
        bass->writeByte(10)->writeByte(120)->writeByte(0)->writeByte(FmLowPass);
    }
    var chords = NEW_(Stream);
    {
        //amAdsrAmp, amAdsrDc, amAdsrAtk, amAdsrDec, amAdsrSus, amAdsrRel
        chords->writeFloat(0.8f)->writeFloat(0.0f)->writeByte(62)->writeByte(128)->writeByte(120)->writeByte(162);
        //fmAdsrAmp, fmAdsrDc, fmAdsrAtk, fmAdsrDec, fmAdsrSus, fmAdsrRel
        chords->writeFloat(0.0f)->writeFloat(0.0f)->writeByte(0)->writeByte(0)->writeByte(0)->writeByte(0);
        //pmAdsrAmp, pmAdsrDc, pmAdsrAtk, pmAdsrDec, pmAdsrSus, pmAdsrRel
        chords->writeFloat(0.2f)->writeFloat(0.0f)->writeByte(16)->writeByte(64)->writeByte(120)->writeByte(80);
        //ftAdsrAmp, ftAdsrDc, ftAdsrAtk, ftAdsrDec, ftAdsrSus, ftAdsrRel
        chords->writeFloat(0.2f)->writeFloat(0.0f)->writeByte(32)->writeByte(42)->writeByte(200)->writeByte(122);

        //amLfoAmp, amLfoFre
        chords->writeFloat(0.2f)->writeFloat(4.1f);
        //fmLfoAmp, fmLfoFre
        chords->writeFloat(3.2f)->writeFloat(4.8f);

        //osc1Amp, osc1Fre, osc1Note, osc1Tune, osc1Psw, osc1Wave
        chords->writeByte(160)->writeFloat(-1.6f)->writeByte(0)->writeByte(0)->writeByte(60)->writeByte(WfSaw);
        //osc2Amp, osc2Fre, osc2Note, osc2Tune, osc2Psw, osc2Wave
        chords->writeByte(100)->writeFloat(1.4f)->writeByte(0)->writeByte(12)->writeByte(220)->writeByte(WfSaw);

        //flt1Cut, flt1Res, flt1Mod, flt1Mode
        chords->writeByte(20)->writeByte(40)->writeByte(0)->writeByte(FmBandPass);
    }
    var mono = NEW_(Stream);
    {
        //amAdsrAmp, amAdsrDc, amAdsrAtk, amAdsrDec, amAdsrSus, amAdsrRel
        mono->writeFloat(1.0f)->writeFloat(0.0f)->writeByte(2)->writeByte(28)->writeByte(60)->writeByte(22);
        //fmAdsrAmp, fmAdsrDc, fmAdsrAtk, fmAdsrDec, fmAdsrSus, fmAdsrRel
        mono->writeFloat(0.0f)->writeFloat(0.0f)->writeByte(0)->writeByte(0)->writeByte(0)->writeByte(0);
        //pmAdsrAmp, pmAdsrDc, pmAdsrAtk, pmAdsrDec, pmAdsrSus, pmAdsrRel
        mono->writeFloat(0.2f)->writeFloat(0.0f)->writeByte(20)->writeByte(64)->writeByte(128)->writeByte(40);
        //ftAdsrAmp, ftAdsrDc, ftAdsrAtk, ftAdsrDec, ftAdsrSus, ftAdsrRel
        mono->writeFloat(0.7f)->writeFloat(0.0f)->writeByte(2)->writeByte(22)->writeByte(40)->writeByte(32);

        //amLfoAmp, amLfoFre
        mono->writeFloat(0.1f)->writeFloat(4.1f);
        //fmLfoAmp, fmLfoFre
        mono->writeFloat(1.2f)->writeFloat(4.8f);

        //osc1Amp, osc1Fre, osc1Note, osc1Tune, osc1Psw, osc1Wave
        mono->writeByte(160)->writeFloat(0.0f)->writeByte(0)->writeByte(0)->writeByte(100)->writeByte(WfSaw);
        //osc2Amp, osc2Fre, osc2Note, osc2Tune, osc2Psw, osc2Wave
        mono->writeByte(120)->writeFloat(0.2f)->writeByte(0)->writeByte(12)->writeByte(80)->writeByte(WfSaw);

        //flt1Cut, flt1Res, flt1Mod, flt1Mode
        mono->writeByte(10)->writeByte(100)->writeByte(0)->writeByte(FmLowPass);
    }
    var piano = NEW_(Stream);
    {
        //amAdsrAmp, amAdsrDc, amAdsrAtk, amAdsrDec, amAdsrSus, amAdsrRel
        piano->writeFloat(1.0f)->writeFloat(0.0f)->writeByte(0)->writeByte(30)->writeByte(80)->writeByte(20);
        //fmAdsrAmp, fmAdsrDc, fmAdsrAtk, fmAdsrDec, fmAdsrSus, fmAdsrRel
        piano->writeFloat(0.0f)->writeFloat(0.0f)->writeByte(0)->writeByte(0)->writeByte(0)->writeByte(0);
        //pmAdsrAmp, pmAdsrDc, pmAdsrAtk, pmAdsrDec, pmAdsrSus, pmAdsrRel
        piano->writeFloat(0.2f)->writeFloat(0.0f)->writeByte(0)->writeByte(24)->writeByte(128)->writeByte(120);
        //ftAdsrAmp, ftAdsrDc, ftAdsrAtk, ftAdsrDec, ftAdsrSus, ftAdsrRel
        piano->writeFloat(0.6f)->writeFloat(0.0f)->writeByte(2)->writeByte(32)->writeByte(60)->writeByte(42);

        //amLfoAmp, amLfoFre
        piano->writeFloat(0.1f)->writeFloat(2.4f);
        //fmLfoAmp, fmLfoFre
        piano->writeFloat(0.4f)->writeFloat(1.1f);

        //osc1Amp, osc1Fre, osc1Note, osc1Tune, osc1Psw, osc1Wave
        piano->writeByte(100)->writeFloat(0.0f)->writeByte(0)->writeByte(0)->writeByte(190)->writeByte(WfTriangle);
        //osc2Amp, osc2Fre, osc2Note, osc2Tune, osc2Psw, osc2Wave
        piano->writeByte(80)->writeFloat(3.1f)->writeByte(0)->writeByte(12)->writeByte(180)->writeByte(WfTriangle);

        //flt1Cut, flt1Res, flt1Mod, flt1Mode
        piano->writeByte(10)->writeByte(120)->writeByte(0)->writeByte(FmLowPass | FmBandPass);
    }

    Synth synth(NULL, 1);
    Synth::defaultSoundbank_ = synth.createSoundbank();
    Synth::defaultSoundbank_->add("bassline.......", bass->data());
    Synth::defaultSoundbank_->add("chords.........", chords->data());
    Synth::defaultSoundbank_->add("mono...........", mono->data());
    Synth::defaultSoundbank_->add("piano..........", piano->data());
    DEL_(bass);
    DEL_(chords);
    DEL_(mono);
    DEL_(piano);
}

void Synth::cleanUp() {
    DEL_(Synth::defaultSoundbank_);
}