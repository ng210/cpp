#include "math.h"
#include "math.h"
#include "base/memory.h"
#include "utils/utils.h"
#include "synth/src/module/drums.h"
#include "synth/src/module/synth-def.h"

NS_FW_BASE_USE
using namespace SYNTH;

#define SMPSINUS(timer) sin(SYNTH_THETA * timer)
#define SMPPULSE(timer) (timer <= 0.5 ? 1.0 : -1.0)
#define SMPNOISE(timer, delta, store) (timer < delta ? store = (float)Utils::randomSigned() : store)

#pragma region GenericDrum
GenericDrum::GenericDrum() : Module((PotBase*)&controls, GenericDrumCtrlCount) {
    memset(timers, 0, sizeof(float) * 6);
    noise = 0.0f;
    for (var i = 0; i < 4; i++) {
        envelopes[i].assignControls((PotBase*)&controls.dahr[i]);
    }
    controls.dahr[1].amp.max = 440.0f;
    for (var i = 0; i < 2; i++) {
        filters[i].assignControls((PotBase*)&controls.flt[i]);
    }
    controls.type.init(0, DefaultDrumType, 1, 0);
    controls.type.set.add(this, GenericDrum::typeSetter);
    for (var i = 0; i < 6; i++) {
        controls.pots.gdFre[i].init(0.0f, 0.5f * *Elem::samplingRate, 1.0f, 0.0f);
        controls.pots.gdAmp[i].init(0, 255, 1, 80);
    }
    //controls.fre[3].init(3000.0f, 9000.0f, 1.0f, 7891.0f);

    setProgram.clear();
    setProgram.add(this, &GenericDrum::programSetter);
}
GenericDrum::~GenericDrum() {
}

bool GenericDrum::isActive() {
    return envelopes[0].isActive();
}

void GenericDrum::setGate(byte velocity) {
    for (var i = 0; i < 4; i++) {
        envelopes[i].setGate(velocity);
    }
    for (var i = 0; i < 6; i++) {
        timers[i] = 0.0;
    }
}

float GenericDrum::runFilter(Flt* flt, float cut, float input) {
    var st = flt->stage(0);
    var q = flt->controls()->res.value.f;
    var f = Flt::cutoffTable[flt->controls()->cut.value.b] + cut;
    var fb = q + q / (1.0f - f);
    st->ai_[0] = st->ai_[0] + f * (input - st->ai_[0] + fb * (st->ai_[0] - st->ai_[1]));
    st->ai_[1] = st->ai_[1] + f * (st->ai_[0] - st->ai_[1]);
    return (float)st->ai_[1];
}

void GenericDrum::renderDefault(float* buffer, int start, int end) {
    for (var i = start; i < end; i++) {
        // run envelopes
        // - env1: main AM
        // - env2: main FM
        // - env3: click AM
        // - env4: cutoff
        float am = envelopes[0].run();
        float fm = envelopes[1].run();
        float cl = envelopes[2].run();
        float cm = envelopes[3].run();

        // AM
        var amp = am + envelopes[0].controls()->amp.value.f * cl;

        // FM
        var fre1 = controls.pots.gdFre[0].value.f;
        var fre2 = controls.pots.gdFre[1].value.f;
        var fre3 = controls.pots.gdFre[2].value.f;
        //var fre4 = controls.pots.gdFre[3].value.f;

        var delta1 = (fre1 * (1.0f + fm)) / *Elem::samplingRate;
        var delta2 = (fre2 * (1.0f + fm)) / *Elem::samplingRate;
        var delta3 = (fre3 * (1.0f + cl)) / *Elem::samplingRate;
        //var delta4 = (fre4 * (1.0f + cl)) / *Elem::samplingRate;
        var smp = controls.pots.gdAmp[0].value.f * (float)SMPSINUS(timers[0]);
            smp += controls.pots.gdAmp[1].value.f * (float)SMPSINUS(timers[1]);
        var click = (float)SMPPULSE(timers[2]);
            noise = click * (float)Utils::randomSigned();
            //SMPNOISE(timers[3], delta4, noise);
            //smp += cl * click;
        if ((timers[0] += delta1) > 1.0f) timers[0] -= 1.0f;
        if ((timers[1] += delta2) > 1.0f) timers[1] -= 1.0f;
        if ((timers[2] += delta3) > 1.0f) timers[2] -= 1.0f;
        //if ((timers[3] += delta4) > 1.0f) timers[3] -= 1.0f;

        // run filter2
        filters[0].update(cm);
        filters[1].update(cm);
        var flt1 = filters[0].run(cl * click * (float)noise);
        smp += controls.pots.gdAmp[2].value.f * filters[1].run(flt1);
        buffer[i] += amp * smp;
    }
}

void GenericDrum::renderHihat(float* buffer, int start, int end) {
    for (var i = start; i < end; i++) {
        // run envelopes
        // - env1: main AM
        // - env2: main FM
        // - env3: click AM
        // - env4: cutoff
        float am = envelopes[0].run();
        float fm = envelopes[1].run();
        float cl = envelopes[2].run();
        float cm = envelopes[3].run();

        var amp = am * (1.0f + cl);
        //cut += click;

        // create 6 pulses
        double smp = 0.0f;
        float delta;
        for (var i = 0; i < 6; i++) {
            delta = (controls.pots.gdFre[i].value.f + fm) / *Elem::samplingRate;
            smp += controls.pots.gdAmp[i].value.f * SMPPULSE(timers[i]);
            timers[i] += delta;
            if (timers[i] > 1.0f) timers[i] -= 1.0f;
        }

        // run filter2
        filters[0].update(cm);
        filters[1].update(cm);
        var flt1 = filters[0].run((float)smp);
        buffer[i] += amp * filters[1].run(flt1);
    }
}

void GenericDrum::renderClap(float* buffer, int start, int end) {
    if (envelopes[0].phase() == EnvPhase::Up) {
        attribute1(1);
    }
    if (attribute1() != 0) {
        for (var i = start; i < end; i++) {

            float am = envelopes[0].run() + envelopes[1].run() + envelopes[2].run() + envelopes[3].run();

            var delta = getControl(gdFreq1)->value.f / *Elem::samplingRate;
            float sin1 = getControl(gdAmp1)->value.f * (float)SMPSINUS(timers[0]);
            timers[0] += delta; if (timers[0] > 1.0f) timers[0] -= 1.0f;

            delta = getControl(gdFreq2)->value.f / *Elem::samplingRate;
            float sin2 = getControl(gdAmp2)->value.f * (float)SMPSINUS(timers[1]);
            timers[1] += delta; if (timers[1] > 1.0f) timers[1] -= 1.0f;

            delta = getControl(gdFreq3)->value.f / *Elem::samplingRate;
            SMPNOISE(delta, timers[2], noise);
            timers[2] += delta; if (timers[2] > 1.0f) timers[2] -= 1.0f;

            var smp = sin1 + sin2 + getControl(gdAmp3)->value.f * noise;

            filters[0].update(0.0f);
            filters[1].update(am);
            var hp = filters[0].run((float)smp);
            var lp = filters[1].run(am * hp);
            buffer[i] += lp;
        }
        if (envelopes[3].phase() == EnvPhase::Up) {
            attribute1(2);
        }
        if (attribute1() == 2 && envelopes[3].phase() == EnvPhase::Idle) {
            attribute1(0);
        }
    }
}

int GenericDrum::typeSetter(void* obj, DrumTypes value, void* unused) {
    var drum = (GenericDrum*)obj;
    drum->render = &GenericDrum::renderDefault;
    PotBase* ctrl;
    switch (value) {
        case HihatType:
            drum->render = &GenericDrum::renderHihat;
            //ctrl = drum->getControl(gdDahr2Amp); ctrl->min = 0.0f; ctrl->max = 880.0f;
            //ctrl = drum->getControl(gdDahr4Amp); ctrl->min = -1.0f; ctrl->max = 1.0f;
            ctrl = drum->getControl(gdFreq1); ctrl->min = 30.0f; ctrl->max = 440.0f;
            ctrl = drum->getControl(gdFreq2); ctrl->min = 30.0f; ctrl->max = 440.0f;
            drum->filters[0].createStages(6);
            drum->filters[1].createStages(2);
            break;
        case ClapType:
            drum->render = &GenericDrum::renderClap;
        case BassDrumType:
        default:
            //ctrl = drum->getControl(gdDahr2Amp); ctrl->min = 0.0f; ctrl->max = 880.0f;
            //ctrl = drum->getControl(gdDahr4Amp); ctrl->min = 0.0f; ctrl->max = 1.0f;
            //drum->getControl(gdDahr4Amp)->init(0.0f, 1.0f, 0.1f, 0.0f);
            ctrl = drum->getControl(gdFreq1); ctrl->min = 30.0f; ctrl->max = 440.0f;
            ctrl = drum->getControl(gdFreq2); ctrl->min = 30.0f; ctrl->max = 440.0f;
            drum->filters[0].createStages(2);
            drum->filters[1].createStages(2);
            break;
    }
    return 1;
}

int GenericDrum::programSetter(void* obj, int ix, void* unused) {
    return Module::programSetter(obj, ix);
}
#pragma endregion


Soundbank* Drums::defaultSoundbank_ = NULL;

Drums::Drums() : Module(NULL, GenericDrumCtrlCount) {
    createOutputBuffers(1);
    setSoundbank.clear();
    setSoundbank.add(this, &Drums::soundbankSetter);
    setProgram.clear();
    setProgram.add(this, &Drums::programSetter);
    drums = drums_;
    setSoundbank(getDefaultSoundbank());

}

Drums::~Drums() {
}

void Drums::initializeFromStream(byte** pData) {
    var hasData = pData != NULL && *pData != NULL;
    for (var i = 0; i < DrumsCount; i++) {
        int prg = 0;
        if (hasData) {
            prg = READ(*pData, byte);
        }
        drums[i].setProgram(prg);
    }
}

Soundbank* Drums::createSoundbank() {
    return drums_[0].createSoundbank();
}

float* Drums::getOutput(int id) {
    return outputs_[0];
}

int Drums::soundbankSetter(void* obj, Soundbank* sb, void* unused) {
    var mdl = (Drums*)obj;
    mdl->soundbank_ = sb;
    for (var i = 0; i < DrumsCount; i++) {
        mdl->drums_[i].setSoundbank(sb);
    }
    return 1;
}

int Drums::programSetter(void* obj, int prgId, void* unused) {
    var drums = (Drums*)obj;
    // hibyte of prgId selects drum
    var drmId = ((word)prgId) >> 8;
    prgId &= 0xff;
    drums->drums_[drmId].setProgram(prgId);
    return 1;
}    

void Drums::setControl(int id, S value) {
    //switch (id)
    //{
    //    case bdTone: break;
    //    case bdTune: break;
    //    case bdDecay: break;
    //    case sdDecay: break;
    //    case sdTune: break;
    //    case sdSnappy: break;
    //    case sdTone: break;
    //    case hhTone:
    //        controls_.ch.flt1.cut.value.b = value.b;
    //        controls_.oh.flt1.cut.value.b = value.b;
    //        value.b += value.b;
    //        controls_.ch.flt2.cut.value.b = value.b;
    //        controls_.oh.flt2.cut.value.b = value.b; 
    //        break;
    //    //case ohDecay: break;
    //    default:
    //        pControls_[id].value = value; break;
    //}
}

void Drums::run(int start, int end) {
    memset(outputs_[0], 0, sizeof(float) * (end - start));
    for (var i = 0; i < DrumsCount; i++) {
        if (drums_[i].isActive()) {
            (drums_[i].*(drums_[i].render))(outputs_[0], start, end);
        }
    }
}

void Drums::setNote(byte note, byte velocity) {
    // C1:BD  D1:SD  E1:LT  F1:MT  G1:HT  A1:RS  H1:CL
    // C2:OH  D2:CH  E2:--  F2:--  G2:--  A2:CY  H2:CB
    var ix = note - drBD;
    if (ix >= 0 && ix < DrumsCount) {
        drums_[ix].setGate(velocity);
    }
}

Soundbank* Drums::getDefaultSoundbank() {
    return Drums::defaultSoundbank_;
}

void Drums::prepare() {
    Stream soundbank;
    GenericDrum drum;
    Drums::defaultSoundbank_ = drum.createSoundbank();

    var bd808 = NEW_(Stream, 256);
    {
        bd808->writeByte(BassDrumType);
        //gdDahr1Amp, gdDahr1Del, gdDahr1Atk, gdDahr1Hld, gdDahr1Rel,
        bd808->writeFloat(1.0f)->writeByte(2)->writeByte(2)->writeByte(10)->writeByte(80);
        //gdDahr2Amp, gdDahr2Del, gdDahr2Atk, gdDahr2Hld, gdDahr2Rel,
        bd808->writeFloat(0.2f)->writeByte(0)->writeByte(1)->writeByte(8)->writeByte(30);
        //gdDahr3Amp, gdDahr3Del, gdDahr3Atk, gdDahr3Hld, gdDahr3Rel,
        bd808->writeFloat(0.6f)->writeByte(0)->writeByte(1)->writeByte(0)->writeByte(3);
        //gdDahr4Amp, gdDahr4Del, gdDahr4Atk, gdDahr4Hld, gdDahr4Rel,
        bd808->writeFloat(0.5f)->writeByte(0)->writeByte(0)->writeByte(0)->writeByte(1);

        //gdFlt1Cut, gdFlt1Res, gdFlt1Mode,
        bd808->writeByte(40)->writeByte(0)->writeByte(FmLowPass);
        //gdFlt2Cut, gdFlt2Res, gdFlt2Mode,
        bd808->writeByte(0)->writeByte(0)->writeByte(FmNoPass);

        //gdFreq1, gdFreq2, gdFreq3, gdFreq4, gdFreq5, gdFreq6,
        bd808->writeFloat(48.2f)->writeFloat(48.7f)->writeFloat(201.0f)->writeFloat(0.0f)->writeFloat(0.0f)->writeFloat(0.0f);
        //gdAmp1, gdAmp2, gdAmp3, gdAmp4, gdAmp5, gdAmp6
        bd808->writeByte(180)->writeByte(140)->writeByte(0)->writeByte(0)->writeByte(0)->writeByte(0);
    }
    Drums::defaultSoundbank_->add("bd808..........", bd808->data());
    DEL_(bd808);

    var sd808 = NEW_(Stream, 256);
    {
        sd808->writeByte(BassDrumType);
        //gdDahr1Amp, gdDahr1Del, gdDahr1Atk, gdDahr1Hld, gdDahr1Rel,
        sd808->writeFloat(1.0f)->writeByte(2)->writeByte(0)->writeByte(8)->writeByte(40);
        //gdDahr2Amp, gdDahr2Del, gdDahr2Atk, gdDahr2Hld, gdDahr2Rel,
        sd808->writeFloat(0.2f)->writeByte(2)->writeByte(4)->writeByte(4)->writeByte(10);
        //gdDahr3Amp, gdDahr3Del, gdDahr3Atk, gdDahr3Hld, gdDahr3Rel,
        sd808->writeFloat(0.8f)->writeByte(0)->writeByte(2)->writeByte(2)->writeByte(50);
        //gdDahr4Amp, gdDahr4Del, gdDahr4Atk, gdDahr4Hld, gdDahr4Rel,
        sd808->writeFloat(0.2f)->writeByte(0)->writeByte(2)->writeByte(10)->writeByte(30);

        //gdFlt1Cut, gdFlt1Res, gdFlt1Mode,
        sd808->writeByte(70)->writeByte(80)->writeByte(FmHighPass);
        //gdFlt2Cut, gdFlt2Res, gdFlt2Mode,
        sd808->writeByte(140)->writeByte(140)->writeByte(FmLowPass);

        //gdFreq1, gdFreq2, gdFreq3, gdFreq4, gdFreq5, gdFreq6,
        sd808->writeFloat(166.5f)->writeFloat(236.5)->writeFloat(52.0f)->writeFloat(0.0f)->writeFloat(0.0f)->writeFloat(0.0f);
        //gdAmp1, gdAmp2, gdAmp3, gdAmp4, gdAmp5, gdAmp6
        sd808->writeByte(120)->writeByte(80)->writeByte(200)->writeByte(0)->writeByte(0)->writeByte(0);
    }
    Drums::defaultSoundbank_->add("sd808..........", sd808->data());
    DEL_(sd808);

    var ch808 = NEW_(Stream, 256);
    {
        ch808->writeByte(HihatType);
        //gdDahr1Amp, gdDahr1Del, gdDahr1Atk, gdDahr1Hld, gdDahr1Rel,
        ch808->writeFloat(0.5f)->writeByte(2)->writeByte(2)->writeByte(4)->writeByte(16);
        //gdDahr2Amp, gdDahr2Del, gdDahr2Atk, gdDahr2Hld, gdDahr2Rel,
        ch808->writeFloat(100.0f)->writeByte(0)->writeByte(2)->writeByte(4)->writeByte(8);
        //gdDahr3Amp, gdDahr3Del, gdDahr3Atk, gdDahr3Hld, gdDahr3Rel,
        ch808->writeFloat(0.8f)->writeByte(0)->writeByte(1)->writeByte(4)->writeByte(4);
        //gdDahr4Amp, gdDahr4Del, gdDahr4Atk, gdDahr4Hld, gdDahr4Rel,
        ch808->writeFloat(0.2f)->writeByte(0)->writeByte(0)->writeByte(1)->writeByte(30);

        //gdFlt1Cut, gdFlt1Res, gdFlt1Mode,
        ch808->writeByte(160)->writeByte(50)->writeByte(FmHighPass);
        //gdFlt2Cut, gdFlt2Res, gdFlt2Mode,
        ch808->writeByte(200)->writeByte(127)->writeByte(FmBandPass | FmHighPass);

        //gdFreq1, gdFreq2, gdFreq3, gdFreq4, gdFreq5, gdFreq6,
        ch808->writeFloat(205.2f)->writeFloat(284.0f)->writeFloat(303.0f)->writeFloat(369.2f)->writeFloat(426.0f)->writeFloat(521.0f);
        //gdAmp1, gdAmp2, gdAmp3, gdAmp4, gdAmp5, gdAmp6
        ch808->writeByte(33)->writeByte(35)->writeByte(40)->writeByte(42)->writeByte(42)->writeByte(37);
    }
    ////gdFreq1, gdFreq2, gdFreq3, gdFreq4, gdFreq5, gdFreq6,
    //ch909->writeFloat(205.2f)->writeFloat(284.0f)->writeFloat(303.0f)->writeFloat(369.2f)->writeFloat(426.0f)->writeFloat(521.0f);
    ////gdAmp1, gdAmp2, gdAmp3, gdAmp4, gdAmp5, gdAmp6
    //ch909->writeByte(40)->writeByte(40)->writeByte(41)->writeByte(42)->writeByte(44)->writeByte(48);
    Drums::defaultSoundbank_->add("ch808..........", ch808->data());
    DEL_(ch808);

    var oh808 = NEW_(Stream, 256);
    {
        oh808->writeByte(HihatType);
        //gdDahr1Amp, gdDahr1Del, gdDahr1Atk, gdDahr1Hld, gdDahr1Rel,
        oh808->writeFloat(0.5f)->writeByte(2)->writeByte(2)->writeByte(36)->writeByte(22);
        //gdDahr2Amp, gdDahr2Del, gdDahr2Atk, gdDahr2Hld, gdDahr2Rel,
        oh808->writeFloat(100.0f)->writeByte(0)->writeByte(2)->writeByte(4)->writeByte(8);
        //gdDahr3Amp, gdDahr3Del, gdDahr3Atk, gdDahr3Hld, gdDahr3Rel,
        oh808->writeFloat(0.8f)->writeByte(0)->writeByte(1)->writeByte(2)->writeByte(4);
        //gdDahr4Amp, gdDahr4Del, gdDahr4Atk, gdDahr4Hld, gdDahr4Rel,
        oh808->writeFloat(0.2f)->writeByte(0)->writeByte(0)->writeByte(1)->writeByte(40);

        //gdFlt1Cut, gdFlt1Res, gdFlt1Mode,
        oh808->writeByte(160)->writeByte(50)->writeByte(FmHighPass);
        //gdFlt2Cut, gdFlt2Res, gdFlt2Mode,
        oh808->writeByte(200)->writeByte(127)->writeByte(FmBandPass | FmHighPass);

        //gdFreq1, gdFreq2, gdFreq3, gdFreq4, gdFreq5, gdFreq6,
        oh808->writeFloat(205.2f)->writeFloat(284.0f)->writeFloat(303.0f)->writeFloat(369.2f)->writeFloat(426.0f)->writeFloat(521.0f);
        //gdAmp1, gdAmp2, gdAmp3, gdAmp4, gdAmp5, gdAmp6
        oh808->writeByte(25)->writeByte(27)->writeByte(29)->writeByte(31)->writeByte(33)->writeByte(35);
    }
    Drums::defaultSoundbank_->add("oh808..........", oh808->data());
    DEL_(oh808);
    
    var lt808 = NEW_(Stream, 256);
    {
        lt808->writeByte(BassDrumType);
        //gdDahr1Amp, gdDahr1Del, gdDahr1Atk, gdDahr1Hld, gdDahr1Rel,
        lt808->writeFloat(0.6f)->writeByte(3)->writeByte(2)->writeByte(20)->writeByte(100);
        //gdDahr2Amp, gdDahr2Del, gdDahr2Atk, gdDahr2Hld, gdDahr2Rel,
        lt808->writeFloat(0.0f)->writeByte(0)->writeByte(1)->writeByte(3)->writeByte(80); 
        //gdDahr3Amp, gdDahr3Del, gdDahr3Atk, gdDahr3Hld, gdDahr3Rel,
        lt808->writeFloat(0.8f)->writeByte(0)->writeByte(14)->writeByte(30)->writeByte(200);
        //gdDahr4Amp, gdDahr4Del, gdDahr4Atk, gdDahr4Hld, gdDahr4Rel,
        lt808->writeFloat(0.0f)->writeByte(0)->writeByte(1)->writeByte(1)->writeByte(60);

        //gdFlt1Cut, gdFlt1Res, gdFlt1Mode,
        lt808->writeByte(24)->writeByte(0)->writeByte(FmLowPass);
        //gdFlt2Cut, gdFlt2Res, gdFlt2Mode,
        lt808->writeByte(12)->writeByte(0)->writeByte(FmLowPass);

        //gdFreq1, gdFreq2, gdFreq3, gdFreq4, gdFreq5, gdFreq6,
        lt808->writeFloat(91.0f)->writeFloat(174.5f)->writeFloat(211.0f)->writeFloat(0.0f)->writeFloat(0.0f)->writeFloat(0.0f);
        //gdAmp1, gdAmp2, gdAmp3, gdAmp4, gdAmp5, gdAmp6
        lt808->writeByte(160)->writeByte(80)->writeByte(20)->writeByte(0)->writeByte(0)->writeByte(0);
    }
    var mt808 = NEW_(Stream, 256);
    {
        mt808->writeByte(BassDrumType);
        //gdDahr1Amp, gdDahr1Del, gdDahr1Atk, gdDahr1Hld, gdDahr1Rel,
        mt808->writeFloat(0.6f)->writeByte(3)->writeByte(2)->writeByte(20)->writeByte(100);
        //gdDahr2Amp, gdDahr2Del, gdDahr2Atk, gdDahr2Hld, gdDahr2Rel,
        mt808->writeFloat(0.6f)->writeByte(0)->writeByte(1)->writeByte(3)->writeByte(80);
        //gdDahr3Amp, gdDahr3Del, gdDahr3Atk, gdDahr3Hld, gdDahr3Rel,
        mt808->writeFloat(0.4f)->writeByte(0)->writeByte(14)->writeByte(30)->writeByte(200);
        //gdDahr4Amp, gdDahr4Del, gdDahr4Atk, gdDahr4Hld, gdDahr4Rel,
        mt808->writeFloat(0.3f)->writeByte(0)->writeByte(1)->writeByte(1)->writeByte(60);

        //gdFlt1Cut, gdFlt1Res, gdFlt1Mode,
        mt808->writeByte(13)->writeByte(0)->writeByte(FmLowPass);
        //gdFlt2Cut, gdFlt2Res, gdFlt2Mode,
        mt808->writeByte(1)->writeByte(0)->writeByte(FmLowPass);

        //gdFreq1, gdFreq2, gdFreq3, gdFreq4, gdFreq5, gdFreq6,
        mt808->writeFloat(157.49f)->writeFloat(161.5f)->writeFloat(211.0f)->writeFloat(0.0f)->writeFloat(0.0f)->writeFloat(0.0f);
        //gdAmp1, gdAmp2, gdAmp3, gdAmp4, gdAmp5, gdAmp6
        mt808->writeByte(160)->writeByte(80)->writeByte(20)->writeByte(0)->writeByte(0)->writeByte(0);
    }
    var ht808 = NEW_(Stream, 256);
    {
        ht808->writeByte(BassDrumType);
        //gdDahr1Amp, gdDahr1Del, gdDahr1Atk, gdDahr1Hld, gdDahr1Rel,
        ht808->writeFloat(0.6f)->writeByte(3)->writeByte(2)->writeByte(20)->writeByte(100);
        //gdDahr2Amp, gdDahr2Del, gdDahr2Atk, gdDahr2Hld, gdDahr2Rel,
        ht808->writeFloat(0.6f)->writeByte(0)->writeByte(1)->writeByte(3)->writeByte(80);
        //gdDahr3Amp, gdDahr3Del, gdDahr3Atk, gdDahr3Hld, gdDahr3Rel,
        ht808->writeFloat(0.4f)->writeByte(0)->writeByte(14)->writeByte(30)->writeByte(200);
        //gdDahr4Amp, gdDahr4Del, gdDahr4Atk, gdDahr4Hld, gdDahr4Rel,
        ht808->writeFloat(0.3f)->writeByte(0)->writeByte(1)->writeByte(1)->writeByte(60);

        //gdFlt1Cut, gdFlt1Res, gdFlt1Mode,
        ht808->writeByte(13)->writeByte(0)->writeByte(FmLowPass);
        //gdFlt2Cut, gdFlt2Res, gdFlt2Mode,
        ht808->writeByte(1)->writeByte(0)->writeByte(FmLowPass);

        //gdFreq1, gdFreq2, gdFreq3, gdFreq4, gdFreq5, gdFreq6,
        ht808->writeFloat(187.49f)->writeFloat(193.5f)->writeFloat(211.0f)->writeFloat(0.0f)->writeFloat(0.0f)->writeFloat(0.0f);
        //gdAmp1, gdAmp2, gdAmp3, gdAmp4, gdAmp5, gdAmp6
        ht808->writeByte(160)->writeByte(80)->writeByte(20)->writeByte(0)->writeByte(0)->writeByte(0);
    }

    Drums::defaultSoundbank_->add("lt808..........", lt808->data());
    Drums::defaultSoundbank_->add("mt808..........", mt808->data());
    Drums::defaultSoundbank_->add("ht808..........", ht808->data());
    DEL_(lt808);
    DEL_(mt808);
    DEL_(ht808);

    var cp808 = NEW_(Stream, 256);
    {
        cp808->writeByte(ClapType);
        //gdDahr1Amp, gdDahr1Del, gdDahr1Atk, gdDahr1Hld, gdDahr1Rel,
        cp808->writeFloat(0.51f)->writeByte(0)->writeByte(0)->writeByte(2)->writeByte(10);
        //gdDahr2Amp, gdDahr2Del, gdDahr2Atk, gdDahr2Hld, gdDahr2Rel,
        cp808->writeFloat(0.45f)->writeByte(5)->writeByte(0)->writeByte(2)->writeByte(4);
        //gdDahr3Amp, gdDahr3Del, gdDahr3Atk, gdDahr3Hld, gdDahr3Rel,
        cp808->writeFloat(0.43f)->writeByte(9)->writeByte(0)->writeByte(3)->writeByte(5);
        //gdDahr4Amp, gdDahr4Del, gdDahr4Atk, gdDahr4Hld, gdDahr4Rel,
        cp808->writeFloat(0.47f)->writeByte(12)->writeByte(0)->writeByte(10)->writeByte(30);

        //gdFlt1Cut, gdFlt1Res, gdFlt1Mode,
        cp808->writeByte(56)->writeByte(120)->writeByte(FmHighPass);
        //gdFlt2Cut, gdFlt2Res, gdFlt2Mode,
        cp808->writeByte(160)->writeByte(120)->writeByte(FmBandPass | FmLowPass);

        //gdFreq1, gdFreq2, gdFreq3, gdFreq4, gdFreq5, gdFreq6,
        cp808->writeFloat(221.2f)->writeFloat(0.2f)->writeFloat(8753.0f)->writeFloat(0.0f)->writeFloat(0.0f)->writeFloat(0.0f);
        //gdAmp1, gdAmp2, gdAmp3, gdAmp4, gdAmp5, gdAmp6
        cp808->writeByte(0)->writeByte(0)->writeByte(180)->writeByte(0)->writeByte(0)->writeByte(0);
    }
    Drums::defaultSoundbank_->add("cp808..........", cp808->data());
    DEL_(cp808);

    //var cp808 = NEW_(Stream, 256);
    //{
    //    // envelopes
    //    cp808->writeByte(4);
    //    // dahr1
    //    cp808->writeFloat(0.51f)->writeFloat(0.0f)->writeByte(0)->writeByte(0)->writeByte(2)->writeByte(5);
    //    // dahr2
    //    cp808->writeFloat(0.45f)->writeFloat(0.0f)->writeByte(5)->writeByte(0)->writeByte(2)->writeByte(4);
    //    // dahr3
    //    cp808->writeFloat(0.43f)->writeFloat(0.0f)->writeByte(9)->writeByte(0)->writeByte(3)->writeByte(5);
    //    // dahr4
    //    cp808->writeFloat(0.47f)->writeFloat(0.0f)->writeByte(12)->writeByte(0)->writeByte(10)->writeByte(90);
    //    // filter
    //    cp808->writeByte(2);
    //    cp808->writeByte(46) /*=Tone*/->writeByte(180)->writeByte(0)->writeByte(FmHighPass);
    //    cp808->writeByte(160) /*=Tone*/->writeByte(120)->writeByte(0)->writeByte(FmLowPass | FmBandPass);
    //    // frequences
    //    cp808->writeByte(3);
    //    cp808->writeFloat(221.0f)->writeByte(0)->writeFloat(0.2f)->writeByte(0)->writeFloat(8753.0f)->writeByte(180);
    //}
    //count++;

    //DEL_(cp808);
}

void Drums::cleanUp() {
    DEL_(Drums::defaultSoundbank_);
    Drums::defaultSoundbank_ = NULL;
}