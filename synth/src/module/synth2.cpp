#include "base/memory.h"
#include "synth/src/module/synth2.h"

NS_FW_BASE_USE
using namespace SYNTH;

Soundbank* Synth2::defaultSoundbank_ = NULL;

Synth2::Synth2(int count) : SynthBase((PotBase*)&controls, Synth2CtrlCount) {
    setupVoice = (PVOICEHANDLER)&Synth2::setupVoiceHandler;
    renderVoice = (PVOICERENDERER)&Synth2::renderVoiceHandler;
    freeVoice = (PVOICEHANDLER)&Synth2::freeVoiceHandler;
    setNoteVoice = (PVOICESETNOTEHANDLER)&Synth2::setNoteVoiceHandler;
    SynthBase::initialize(count);
    program_ = -1;
    createOutputBuffers(1);
    setSoundbank(getDefaultSoundbank());

    // bind filter 1 and 2
    controls.flt11.cut.set.add(&controls.flt12.cut, Synth2::cutoffSetter);
    controls.flt21.cut.set.add(&controls.flt22.cut, Synth2::cutoffSetter);
    controls.flt31.cut.set.add(&controls.flt32.cut, Synth2::cutoffSetter);

}

Synth2::~Synth2() {
}

//float Synth2::applyFilter(Flt* flt, float smp, float cut) {
//    var st = flt->stage(0);
//    var q = flt->controls()->res.value.f;
//    var f = Flt::cutoffTable[flt->controls()->cut.value.b] + cut;
//    var fb = q + q / (1.0f - f);
//    st->ai_[0] = st->ai_[0] + f * (smp - st->ai_[0] + fb * (st->ai_[0] - st->ai_[1]));
//    st->ai_[1] = st->ai_[1] + f * (st->ai_[0] - st->ai_[1]);
//    return (float)st->ai_[1];
//}

// Voice handling
void Synth2::setupVoiceHandler(Voice& v) {
    // Envelopes
    v.envelopes = NEWARR(Adsr, 4);
    for (var ei = 0; ei < 4; ei++) {
        v.envelopes[ei].assignControls((PotBase*)&((AdsrCtrls*)&controls.amAdsr)[ei]);
    }
    //controls.fmAdsr.amp.init(0.0f, 440.0f, 0.1f, 0.0f);
    // LFOs
    v.lfos = NEWARR(Lfo, 4);
    for (var li = 0; li < 4; li++) {
        v.lfos[li].assignControls((PotBase*)&((LfoCtrls*)&controls.amLfo)[li]);
    }
    //v.lfos[0].assignControls((PotBase*)&controls.amLfo);
    //v.lfos[1].assignControls((PotBase*)&controls.fmLfo);
    //controls.fmLfo.amp.init(0.0f, 20.0f, 0.1f, 0.0f);
    // Oscillators
    v.oscillators = NEWARR(Osc, 2);
    v.oscillators[0].setNoteControl(&v.note);
    v.oscillators[1].setNoteControl(&v.note);
    v.oscillators[0].assignControls((PotBase*)&controls.osc1);
    v.oscillators[1].assignControls((PotBase*)&controls.osc2);
    // Filter
    v.filters = NEWARR(Flt, 6);
    for (var fi = 0; fi < 6; fi++) {
        v.filters[fi].createStages(2);
        v.filters[fi].assignControls((PotBase*)&((FltCtrls*)&controls.flt11)[fi]);
    }

    // pots
    v.pots = (PotBase**)&controls.pots;
}
void Synth2::renderVoiceHandler(Voice& v, float* buffer, int start, int end) {
    var pots = (Synth2CtrlsPots*)v.pots;
    for (var i = start; i < end; i++) {
        // read feedbacks
        float osc1 = (float)v.oscillators[0].smp;
        float osc2 = (float)v.oscillators[1].smp;

        // run LFOs
        var lfoAm = 0.5f * (v.lfos[0].run() + 1.0f);
        var lfoFm = v.lfos[1].run();
        var lfoPm = v.lfos[2].run();
        var lfoCm = v.lfos[3].run();

        // run envelopes
        var envAm = v.envelopes[0].run();
        var envFm = v.envelopes[1].run();
        var envPm = v.envelopes[2].run();
        var envCm = v.envelopes[3].run();

        // AM
        var am1 = pots->amOsc1.value.f;
        var amOsc1 = osc2 * am1 + 1.0f - am1;
        var am2 = pots->amOsc2.value.f;
        var amOsc2 = osc1 * am2 + 1.0f - am2;
        
        // FM
        var fmOsc1 = osc2 * pots->fmOsc1.value.f + 1.0f;
        var fm1 = fmOsc1 + lfoFm + envFm + 1.0f;
        var fmOsc2 = osc1 * pots->fmOsc2.value.f + 1.0f;
        var fm2 = fmOsc2 + lfoFm + envFm + 1.0f;

        // PM
        var pm1 = pots->pmOsc1.value.f;
        var pmOsc1 = osc2 * pm1 + 1.0f - pm1;
        pm1 = lfoPm;    // pmOsc1* (lfoPm + envPm);
        var pm2 = pots->pmOsc2.value.f;
        var pmOsc2 = osc1 * pm2 + 1.0f - pm2;
        pm2 = pmOsc2 * (lfoPm + envPm);

        // CM
        var cm = envCm + lfoCm;

        // run oscillators
        float args[2] = { fm1, pm1 };
        var smp = v.oscillators[0].run(args) * amOsc1;
        args[0] = fm2; args[1] = pm2;
        smp += v.oscillators[1].run(args) * amOsc2;

        float output = 0.0f;
        // run filters
        v.filters[0].update(cm);
        v.filters[1].update(cm + pots->flt1Band.value.f);
        v.filters[2].update(cm);
        v.filters[3].update(cm + pots->flt2Band.value.f);
        v.filters[4].update(cm);
        v.filters[5].update(cm + pots->flt3Band.value.f);

        var f1 = v.filters[0].run(smp);
        var f2 = v.filters[2].run(smp);
        var f3 = v.filters[4].run(smp);
        output += v.filters[1].run(f1) * pots->flt1Amp.value.f;
        output += v.filters[3].run(f2) * pots->flt2Amp.value.f;
        output += v.filters[5].run(f3) * pots->flt3Amp.value.f;

        buffer[i] += lfoAm * envAm * output;
    }

//    var pots = (Synth2CtrlsPots*)v.pots;
//    for (var i = start; i < end; i++) {
//        // read feedbacks
//        float osc1 = (float)v.oscillators[0].smp;
//        float osc2 = (float)v.oscillators[1].smp;
//
//        // run LFOs
//        var lfoAm = v.lfos[0].run() + 1.0f;
//        var lfoFm = v.lfos[1].run() + 1.0f;
//        var lfoPm = v.lfos[2].run() + 1.0f;
//        var lfoCm = v.lfos[3].run();
//
//        // run envelopes
//        var envAm = v.envelopes[0].run();
//        var envFm = v.envelopes[1].run();
//        var envPm = v.envelopes[2].run();
//        var envCm = v.envelopes[3].run();
//
//        // AM
//        var am1 = pots->amOsc1.value.f;
//        var amOsc1 = osc2 * am1 + 1.0f - am1;
//        var am2 = pots->amOsc2.value.f;
//        var amOsc2 = osc1 * am2 + 1.0f - am2;
//
//        // FM
//        var fm1 = pots->fmOsc1.value.f;
//        var fmOsc1 = osc2 * fm1 + 1.0f;
//        fm1 = fmOsc1 * lfoFm + envFm;
//        var fm2 = pots->fmOsc2.value.f;
//        var fmOsc2 = osc1 * fm2 + 1.0f - fm2;
//        fm2 = fmOsc2 * lfoFm + envFm;
//
//        // PM
//        var pm1 = pots->pmOsc1.value.f;
//        var pmOsc1 = osc2 * pm1 + 1.0f - pm1;
//        pm1 = pmOsc1 * (lfoPm + envPm);
//        var pm2 = pots->pmOsc2.value.f;
//        var pmOsc2 = osc1 * pm2 + 1.0f - pm2;
//        pm2 = pmOsc2 * (lfoPm + envPm);
//
//        // CM
//        var cm = envCm + lfoCm;
//
//    fm1 = pm1 = fm2 = pm2 = 0.0f;
//
//        // run oscillators
//        float args[2] = { fm1, pm1 };
//        float smp1 = v.oscillators[0].run(args);
//buffer[i] += smp1;
//        smp1 *= amOsc1 * pots->osc1Amp.value.f;
//        
//        args[0] = fm2; args[1] = pm2;
//        float smp2 = v.oscillators[1].run(args);
//        smp2 *= amOsc2 * pots->osc1Amp.value.f;
//
//        float smp = smp1 + smp2;
//
//        // run filters
//        v.filters[0].update(cm);
//        var output = v.filters[0].run(smp) * pots->flt1Amp.value.f;
//        v.filters[1].update(cm);
//        output += v.filters[1].run(smp) * pots->flt2Amp.value.f;
//        v.filters[2].update(cm);
//        output += v.filters[2].run(smp) * pots->flt3Amp.value.f;
//
//        //buffer[i] += lfoAm * envAm * output;
//    }
}
void Synth2::freeVoiceHandler(Voice& v) {
    DELARR(v.envelopes);
    DELARR(v.lfos);
    DELARR(v.oscillators);
    DELARR(v.filters);
}
void Synth2::setNoteVoiceHandler(Voice& v, byte note, byte velocity) {
    v.velocity.value = velocity / 255.0f;
    v.note.value = note;
    for (var i = 0; i < 4; i++) v.lfos[i].reset();
    for (var i = 0; i < 4; i++) v.envelopes[i].setGate(velocity);
}

Soundbank* Synth2::getDefaultSoundbank() {
    return Synth2::defaultSoundbank_;
}

int Synth2::cutoffSetter(void* cut, S value, void* unused) {
    ((Pot*)cut)->set(value);
    return 1;
}

//Module
void Synth2::prepare() {
    var synth1 = NEW_(Stream);
    {
        //amAdsrAmp, amAdsrAtk, amAdsrDec, amAdsrSus, amAdsrRel
        synth1->writeFloat(1.0f)->writeByte(12)->writeByte(22)->writeByte(120)->writeByte(22);
        //fmAdsrAmp, fmAdsrAtk, fmAdsrDec, fmAdsrSus, fmAdsrRel
        synth1->writeFloat(0.2f)->writeByte(0)->writeByte(30)->writeByte(80)->writeByte(20);
        //pmAdsrAmp, pmAdsrAtk, pmAdsrDec, pmAdsrSus, pmAdsrRel
        synth1->writeFloat(0.05f)->writeByte(1)->writeByte(24)->writeByte(168)->writeByte(44);
        //cmAdsrAmp, ftAdsrAtk, ftAdsrDec, ftAdsrSus, ftAdsrRel
        synth1->writeFloat(0.01f)->writeByte(12)->writeByte(22)->writeByte(100)->writeByte(32);

        //amLfoAmp, amLfoFre
        synth1->writeFloat(0.01f)->writeFloat(4.5f);
        //fmLfoAmp, fmLfoFre
        synth1->writeFloat(0.02f)->writeFloat(4.8f);
        //pmLfoAmp, pmLfoFre
        synth1->writeFloat(0.03f)->writeFloat(2.2f);
        //cmLfoAmp, cmLfoFre
        synth1->writeFloat(0.001f)->writeFloat(1.7f);

        //osc1Amp, osc1Fre, osc1Note, osc1Tune, osc1Psw, osc1Wave
        synth1->writeByte(200)->writeFloat(0.0f)->writeByte(0)->writeByte(0)->writeByte(240)->writeByte(WfTriangle);
        //osc2Amp, osc2Fre, osc2Note, osc2Tune, osc2Psw, osc2Wave
        synth1->writeByte(0)->writeFloat(0.4f)->writeByte(0)->writeByte(0)->writeByte(220)->writeByte(WfTriangle);

        //flt11Cut, flt11Res, flt11Mode
        synth1->writeByte(3)->writeByte(100)->writeByte(FmHighPass);
        //flt12Cut, flt12Res, flt12Mode
        synth1->writeByte(5)->writeByte(180)->writeByte(FmLowPass);
        //flt21Cut, flt21Res, flt21Mode
        synth1->writeByte(9)->writeByte(160)->writeByte(FmHighPass);
        //flt22Cut, flt22Res, flt22Mode
        synth1->writeByte(12)->writeByte(180)->writeByte(FmLowPass);
        //flt31Cut, flt31Res, flt31Mode
        synth1->writeByte(21)->writeByte(180)->writeByte(FmHighPass);
        //flt32Cut, flt32Res, flt32Mode
        synth1->writeByte(26)->writeByte(100)->writeByte(FmLowPass);

        // amOsc1, amOsc2, fmOsc1, fmOsc2, pmOsc1, pmOsc2
        synth1->writeByte(255)->writeByte(0)->writeFloat(0.0f)->writeFloat(0.0f)->writeByte(0)->writeByte(0);
        // flt1Amp, flt2Amp, flt3Amp
        synth1->writeByte(120)->writeByte(160)->writeByte(100);
        // flt1Band, flt2Band, flt3Band
        synth1->writeByte(2)->writeByte(3)->writeByte(5);
    }
    Synth2 synth(1);
    Synth2::defaultSoundbank_ = synth.createSoundbank();
    Synth2::defaultSoundbank_->add("synth1.........", synth1->data());
    DEL_(synth1);
}

void Synth2::cleanUp() {
    DEL_(Synth2::defaultSoundbank_);
    Synth2::defaultSoundbank_ = NULL;
}