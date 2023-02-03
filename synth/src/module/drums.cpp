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
#define SMPNOISE(delta, timer, store) ((timer < delta || timer > 0.5 && timer < 0.5 + delta) ? store = (float)Utils::randomSigned() : store)

#pragma region GenericDrum
GenericDrum::GenericDrum() {
    memset(timers, 0, sizeof(float) * 6);
    noise = 0.0f;
}
GenericDrum::~GenericDrum() {
}

void GenericDrum::assignControls(PotBase* controls) {
    pControls_ = controls;
}

void GenericDrum::initialize(byte** pData) {
    // 4 EnvCtrls
    // 1+1 FltCtrls
    // 3 float, 3 byte
    var controls = pControls_;
    // read envelopes
    envCount = READ(*pData, byte);
    for (var i = 0; i < envCount; i++) {
        envelopes[i].assignControls(controls);
        envelopes[i].setFromStream(*pData);
        controls += DahrCtrlCount;
    }
    // prepare fm envelope
    envelopes[1].controls()->amp.value.f /= *Elem::samplingRate;
    envelopes[1].controls()->dc.value.f /= *Elem::samplingRate;

    var fltCount = READ(*pData, byte);
    controls = (PotBase*)&((GenericDrumCtrls*)pControls_)->flt1;
    for (var i = 0; i < fltCount; i++) {
        filters[i].assignControls(controls);
        filters[i].setFromStream(*pData);
        controls += FltCtrlCount;
    }

    // read oscillator data
    var oscCount = READ(*pData, byte);
    for (var i = 0; i < oscCount; i++) {
        var f = READ(*pData, float);
        fre[i] = f / *Elem::samplingRate;
        var a = READ(*pData, byte);
        amp[i] = a/255.0f;
    }
}

bool GenericDrum::isActive() {
    return envelopes[0].isActive();
}

void GenericDrum::setProgram(int prgId) {
    var sb = soundBank_;
    program_ = prgId;
    isActive_ = true;
    var count = (int)*sb;
    if (prgId < count) {
        var offset = *(short*)(sb + 1 + 16 * prgId + 14);
        sb += offset;
        initialize(&sb);
    }
}

void GenericDrum::setGate(byte velocity) {
    for (var i = 0; i < envCount; i++) {
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
    return st->ai_[1] = st->ai_[1] + f * (st->ai_[0] - st->ai_[1]);
}

void GenericDrum::render(float* buffer, int start, int end) {
    for (var i = start; i < end; i++) {
        // run envelopes
        // - env1: main AM
        // - env1: main FM
        // - env3: click AM
        // - env4: cutoff
        float env[4];
        for (var ei = 0; ei < 4; ei++) {
            env[ei] = envelopes[ei].run();
        }
        // run oscillators
        // - 1st and 2nd are sinus
        float smp = amp[0] * (float)SMPSINUS(timers[0]) + amp[1] * (float)SMPSINUS(timers[1]);
        // - 3rd is noise
        SMPNOISE(fre[2], timers[2], noise);

        var f = env[1] + fre[1] * env[2];
        if ((timers[0] += f) > 1.0) timers[0] -= 1.0;
        if ((timers[1] += fre[0] + f) > 1.0) timers[1] -= 1.0;
        if ((timers[2] += fre[2]) > 1.0) timers[2] -= 1.0;

        // run 1 filter
        filters[0].update(env[3]);
        smp += amp[2] * filters[0].run((float)noise);
        // smp = runFilter(&filters[0], env[3], smp);
        buffer[i] += (env[0] + env[2]) * smp;
    }
}

void OpenHihat::setProgram(int prgId) {
    GenericDrum::setProgram(prgId);
    var hld = 2 * ((GenericDrumCtrls*)pControls_)->dahr1.rel.value.b;
    if (hld > 255) hld = 255;
    ((GenericDrumCtrls*)pControls_)->dahr1.hld.value.b = hld;
}

#pragma endregion

byte* createDefaultSoundBank() {
    int count = 0;
    void* data[2 * 16];
#pragma region BassDrum
    byte bd808_[] = {
        // envelopes
        4,
        // dahr1
        DF(1.0f), DF(0.0f), DB(1), DB(2), DB(80), DB(180) /*=Decay*/,
        // dahr2
        DF(54.0f), DF(47.0f) /*=Tune*/, DB(0), DB(0), DB(1), DB(60),
        // dahr3
        DF(0.8f) /*=Tone*/, DF(0.0f), DB(0), DB(0), DB(0), DB(2),
        // dahr4
        DF(1.0f), DF(0.0f), DB(0), DB(0), DB(1), DB(2),
        // filter
        1,
        DB(1) /*=Tone*/, DB(80), DB(0), DB(FmLowPass),
        // frequences
        3,
        DF(3.2f), DB(160), DF(273.0f), DB(40), DF(9511.0f), DB(10)
    };
    data[2 * count] = NEW_(Stream, bd808_, arraysize(bd808_));
    data[2 * count + 1] = "bd808........";
    count++;
    byte bd909_[] = {
        // envelopes
        4,
        // dahr1
        DF(1.0f), DF(0.0f), DB(2), DB(6), DB(80), DB(100) /*=Decay*/,
        // dahr2
        DF(169.0f), DF(51.0f) /*=Tune*/, DB(0), DB(0), DB(1), DB(50),
        // dahr3
        DF(0.2f) /*=Tone*/, DF(0.0f), DB(0), DB(0), DB(1), DB(6),
        // dahr4
        DF(1.0f), DF(0.0f), DB(0), DB(0), DB(1), DB(2),
        // filter
        1,
        DB(1) /*=Tone*/, DB(40), DB(0), DB(FmLowPass),
        // frequences
        3,
        DF(3.2f), DB(200), DF(273.0f), DB(20), DF(9511.0f), DB(10)
    };
    data[2 * count] = NEW_(Stream, bd909_, arraysize(bd909_));
    data[2 * count + 1] = "bd909........";
    count++;
    byte bd707_[] = {
        // envelopes
        4,
        // dahr1
        DF(1.0f), DF(0.0f), DB(2), DB(8), DB(20), DB(80) /*=Decay*/,
        // dahr2
        DF(29.0f), DF(54.0f) /*=Tune*/, DB(0), DB(10), DB(10), DB(30),
        // dahr3
        DF(0.4f) /*=Tone*/, DF(0.0f), DB(0), DB(2), DB(4), DB(30),
        // dahr4
        DF(0.4f), DF(0.0f), DB(0), DB(1), DB(0), DB(12),
        // filter
        1,
        DB(2) /*=Tone*/, DB(100), DB(0), DB(FmLowPass),
        // frequences
        3,
        DF(11.2f), DB(160), DF(173.0f), DB(30), DF(9511.0f), DB(40)
    };
    data[2 * count] = NEW_(Stream, bd707_, arraysize(bd707_));
    data[2 * count + 1] = "bd707........";
    count++;
#pragma endregion
#pragma region SnareDrum
    byte sd808_[] = {
        // envelopes
        4,
        // dahr1
        DF(1.0f), DF(0.0f), DB(2), DB(1), DB(40), DB(80) /*=Decay*/,
        // dahr2
        DF(39.0f), DF(171.0f) /*=Tune*/, DB(0), DB(1), DB(0), DB(30),
        // dahr3
        DF(1.0f) /*=Tone*/, DF(0.0f), DB(0), DB(0), DB(0), DB(3),
        // dahr4
        DF(0.2f), DF(0.0f), DB(0), DB(0), DB(4), DB(20),
        // filter
        1,
        DB(60) /*=Tone*/, DB(40), DB(0), DB(FmHighPass),
        // frequences
        3,
        DF(151.0f), DB(80), DF(15.7f), DB(10), DF(6511.0f), DB(80)
    };
    data[2 * count] = NEW_(Stream, sd808_, arraysize(sd808_));
    data[2 * count + 1] = "sd808........";
    count++;
    byte sd909_[] = {
        // envelopes
        4,
        // dahr1
        DF(1.0f), DF(0.0f), DB(2), DB(2), DB(40), DB(60) /*=Decay*/,
        // dahr2
        DF(39.0f), DF(231.0f) /*=Tune*/, DB(0), DB(1), DB(0), DB(20),
        // dahr3
        DF(0.8f) /*=Tone*/, DF(0.0f), DB(0), DB(0), DB(0), DB(4),
        // dahr4
        DF(0.5f), DF(0.0f), DB(0), DB(0), DB(0), DB(80),
        // filter
        1,
        DB(40) /*=Tone*/, DB(60), DB(0), DB(FmBandPass),
        // frequences
        3,
        DF(81.0f), DB(140), DF(105.7f), DB(40), DF(9511.0f), DB(40)
    };
    data[2 * count] = NEW_(Stream, sd909_, arraysize(sd909_));
    data[2 * count + 1] = "sd909........";
    count++;
    byte sda0a_[] = {
        // envelopes
        4,
        // dahr1
        DF(1.0f), DF(0.0f), DB(10), DB(6), DB(20), DB(100) /*=Decay*/,
        // dahr2
        DF(29.0f), DF(175.0f) /*=Tune*/, DB(0), DB(0), DB(10), DB(10),
        // dahr3
        DF(0.8f) /*=Tone*/, DF(0.0f), DB(0), DB(3), DB(1), DB(6),
        // dahr4
        DF(-0.2f), DF(0.0f), DB(8), DB(1), DB(2), DB(20),
        // filter
        1,
        DB(72) /*=Tone*/, DB(80), DB(0), DB(FmBandPass),
        // frequences
        3,
        DF(55.0f), DB(60), DF(17.7f), DB(80), DF(6511.0f), DB(100)
    };
    data[2 * count] = NEW_(Stream, sda0a_, arraysize(sda0a_));
    data[2 * count + 1] = "sda0a........";
    count++;
#pragma endregion
#pragma region Hihat
    byte hh808_[] = {
        // envelopes
        4,
        // dahr1
        DF(1.0f), DF(0.0f), DB(4), DB(4), DB(20), DB(60),
        // dahr2
        DF(127.2f), DF(0.0f), DB(0), DB(8), DB(1), DB(22),
        // dahr3
        DF(0.8f), DF(0.0f), DB(0), DB(1), DB(0), DB(3),
        // dahr4
        DF(-0.2f), DF(0.0f), DB(0), DB(1), DB(2), DB(10),
        // filter
        2,
        DB(70) /*=Tone*/, DB(40), DB(0), DB(FmHighPass),
        DB(142) /*=Tone*/, DB(80), DB(0), DB(FmBandPass | FmHighPass),
        // frequences
        6,
        DF(205.0f), DB(60), DF(284.0f), DB(0), DF(303.0f), DB(0), DF(369.0f), DB(0), DF(426.0f), DB(0), DF(521.0f), DB(0)
    };
    data[2 * count] = NEW_(Stream, hh808_, arraysize(hh808_));
    data[2 * count + 1] = "hh808........";
    count++;
    byte hha0a_[] = {
        // envelopes
        4,
        // dahr1
        DF(1.0f), DF(0.0f), DB(5), DB(2), DB(30), DB(60),
        // dahr2
        DF(103.0f), DF(7.0f), DB(0), DB(1), DB(2), DB(2),
        // dahr3
        DF(0.4f), DF(0.0f), DB(0), DB(2), DB(0), DB(6),
        // dahr4
        DF(-0.1f), DF(0.0f), DB(0), DB(0), DB(2), DB(12),
        // filter
        2,
        DB(59) /*=Tone*/, DB(40), DB(0), DB(FmHighPass),
        DB(127) /*=Tone*/, DB(20), DB(0), DB(FmLowPass | FmBandPass),
        // frequences
        6,
        //DF(205.0f), DB(60), DF(284.0f), DB(0), DF(303.0f), DB(0), DF(369.0f), DB(0), DF(426.0f), DB(0), DF(521.0f), DB(0)
        DF(457.0f), DB(40), DF(211.0f), DB(0), DF(241.0f), DB(0), DF(293.0f), DB(0), DF(367.0f), DB(0), DF(569.0f), DB(0)
    };
    data[2 * count] = NEW_(Stream, hha0a_, arraysize(hha0a_));
    data[2 * count + 1] = "hha0a........";
    count++;
#pragma endregion
#pragma region Tom
    byte lt808_[] = {
        // envelopes
        4,
        // dahr1
        DF(1.0f), DF(0.0f), DB(2), DB(4), DB(10), DB(120) /*=Decay*/,
        // dahr2
        DF(69.0f), DF(88.0f) /*=Tune*/, DB(0), DB(1), DB(0), DB(80),
        // dahr3
        DF(1.0f) /*=Tone*/, DF(0.0f), DB(0), DB(0), DB(0), DB(1),
        // dahr4
        DF(0.2f), DF(0.0f), DB(0), DB(0), DB(0), DB(20),
        // filter
        1,
        DB(4) /*=Tone*/, DB(40), DB(0), DB(FmLowPass | FmBandPass),
        // frequences
        3,
        DF(1.0f), DB(100), DF(205.7f), DB(90), DF(9511.0f), DB(20)
    };
    data[2 * count] = NEW_(Stream, lt808_, arraysize(lt808_));
    data[2 * count + 1] = "lt808........";
    count++;
    byte mt808_[] = {
        // envelopes
        4,
        // dahr1
        DF(1.0f), DF(0.0f), DB(2), DB(4), DB(10), DB(120) /*=Decay*/,
        // dahr2
        DF(65.0f), DF(123.0f) /*=Tune*/, DB(0), DB(1), DB(0), DB(80),
        // dahr3
        DF(1.0f) /*=Tone*/, DF(0.0f), DB(0), DB(0), DB(0), DB(10),
        // dahr4
        DF(0.2f), DF(0.0f), DB(0), DB(0), DB(0), DB(20),
        // filter
        1,
        DB(4) /*=Tone*/, DB(40), DB(0), DB(FmLowPass | FmBandPass),
        // frequences
        3,
        DF(2.7f), DB(110), DF(229.7f), DB(80), DF(9511.0f), DB(20)
    };
    data[2 * count] = NEW_(Stream, mt808_, arraysize(mt808_));
    data[2 * count + 1] = "mt808........";
    count++;
    byte ht808_[] = {
        // envelopes
        4,
        // dahr1
        DF(1.0f), DF(0.0f), DB(2), DB(4), DB(10), DB(120) /*=Decay*/,
        // dahr2
        DF(87.0f), DF(167.0f) /*=Tune*/, DB(0), DB(1), DB(0), DB(140),
        // dahr3
        DF(1.0f) /*=Tone*/, DF(0.0f), DB(0), DB(0), DB(0), DB(4),
        // dahr4
        DF(0.2f), DF(0.0f), DB(0), DB(0), DB(0), DB(20),
        // filter
        1,
        DB(2) /*=Tone*/, DB(0), DB(0), DB(FmLowPass | FmBandPass),
        // frequences
        3,
        DF(3.7f), DB(120), DF(267.7f), DB(70), DF(9511.0f), DB(20)
    };
    data[2 * count] = NEW_(Stream, ht808_, arraysize(ht808_));
    data[2 * count + 1] = "cp808........";
    count++;
#pragma endregion
#pragma region Clap
    byte clap_[] = {
        // envelopes
        4,
        // dahr1
        DF(0.6f), DF(0.0f), DB(0), DB(0), DB(2), DB(5),
        // dahr2
        DF(0.5f), DF(0.0f), DB(10), DB(0), DB(1), DB(8),
        // dahr3
        DF(0.65f), DF(0.0f), DB(21), DB(0), DB(2), DB(5),
        // dahr4
        DF(0.6f), DF(0.0f), DB(28), DB(0), DB(20), DB(100),
        // filter
        2,
        DB(10) /*=Tone*/, DB(210), DB(0), DB(FmHighPass),
        DB(130) /*=Tone*/, DB(20), DB(0), DB(FmLowPass),
        // frequences
        3,
        DF(225.0f), DB(6), DF(374.0f), DB(3), DF(9512.0f), DB(160)
    };
    data[2 * count] = NEW_(Stream, clap_, arraysize(clap_));
    data[2 * count + 1] = "cp808a.......";
    count++;
#pragma endregion

#pragma region soundbank
    word offset = 1 + count * 16;
    Stream* s = NEW_(Stream, 1024);
    s->writeByte(count);
    for (var i = 0; i < count; i++) {
        s->writeString((char*)data[2 * i + 1]);
        s->writeWord(offset); offset += (word)((Stream*)data[2 * i])->length();
    }
    for (var i = 0; i < count; i++) {
        var stream = (Stream*)data[2 * i];
        s->writeStream(stream);
        DEL_(stream);
    }
#pragma endregion

    var bytes = s->extract();
    DEL_(s);

    return bytes;
}

Drums::Drums() {
    pControls_ = (PotBase*)&controls_;
    createOutputBuffers(1);
    soundBank_ = createDefaultSoundBank();

    bassDrum_.assignControls((PotBase*)&controls_.bd);
    bassDrum_.soundBank(soundBank_);
    bassDrum_.setProgram(prgBD808);

    snareDrum_.assignControls((PotBase*)&controls_.sd);
    snareDrum_.filters[0].createStages(2);
    snareDrum_.soundBank(soundBank_);
    snareDrum_.setProgram(prgSD808);

    closedHihat_.assignControls((PotBase*)&controls_.ch);
    closedHihat_.filters[0].createStages(8);
    closedHihat_.filters[1].createStages(8);
    closedHihat_.soundBank(soundBank_);
    closedHihat_.setProgram(prgHH808);

    openHihat_.assignControls((PotBase*)&controls_.oh);
    openHihat_.filters[0].createStages(8);
    closedHihat_.filters[1].createStages(8);
    openHihat_.soundBank(soundBank_);
    openHihat_.setProgram(prgHH808);

    lowTom_.assignControls((PotBase*)&controls_.lt);
    lowTom_.filters[0].createStages(4);
    lowTom_.soundBank(soundBank_);
    lowTom_.setProgram(prgLT808);

    midTom_.assignControls((PotBase*)&controls_.mt);
    midTom_.filters[0].createStages(4);
    midTom_.soundBank(soundBank_);
    midTom_.setProgram(prgMT808);

    highTom_.assignControls((PotBase*)&controls_.ht);
    highTom_.filters[0].createStages(4);
    highTom_.soundBank(soundBank_);
    highTom_.setProgram(prgHT808);

    clap_.assignControls((PotBase*)&controls_.cp);
    closedHihat_.filters[0].createStages(6);
    closedHihat_.filters[1].createStages(4);
    clap_.soundBank(soundBank_);
    clap_.setProgram(prgCP808);
    clap_.envelopes[1].controls()->amp.value.f *= *Elem::samplingRate;
    clap_.envelopes[1].controls()->dc.value.f *= *Elem::samplingRate;
}

Drums::~Drums() {
    FREE(soundBank_);
}

//float* Drums::getInput(int id) {
//    return inputs_[0];
//}

float* Drums::getOutput(int id) {
    return outputs_[0];
}

//void Drums::renderGenericDrum(GenericDrum& drum, float* buffer, int start, int end) {
//    for (var i = start; i < end; i++) {
//        var a = drum.envelopes[0].run();
//        var am1 = a * (3 * a + 1) / (5 - a);
//        var fm = drum.envelopes[1].run();
//        var env = drum.envelopes[2].run();
//
//        var delta = fm / *Osc::samplingRate + drum.fre[0] * env ;
//        var smp = (float)SMPSINUS(drum.timers[0]);
//        drum.timers[0] += delta; if (drum.timers[0] > 1.0) drum.timers[0] -= 1.0;
//
//        float noise = (float)(env * SMPNOISE(drum.fre[1], drum.timers[1], drum.smp));
//
//        //bassDrum_.filter.update(0.2 * adsr);
//        //smp = bassDrum_.filter.run(smp);
//        buffer[i] += am1 * smp + 0.2f * noise;
//    }
//}

void Drums::renderBassDrum(float* buffer, int start, int end) {
    if (bassDrum_.isActive()) {
        bassDrum_.render(buffer, start, end);
    }
}

void Drums::renderSnareDrum(float* buffer, int start, int end) {
    if (snareDrum_.isActive()) {
        snareDrum_.render(buffer, start, end);
        //for (var i = start; i < end; i++) {

        //    float a = snareDrum_.envelopes[0].run();
        //    float am2 = snareDrum_.envelopes[1].run();
        //    var am = a * a; // a* (3 * a + 1) / (5 - a) + 0.2f * am2;
        //    float fm = snareDrum_.envelopes[2].run();
        //    float cut = snareDrum_.envelopes[3].run();

        //    var delta = fm / *Osc::samplingRate + snareDrum_.fre[0];
        //    float sin1 = 0.57f * (float)SMPSINUS(snareDrum_.timers[0]);
        //    snareDrum_.timers[0] += delta; if (snareDrum_.timers[0] > 1.0) snareDrum_.timers[0] -= 1.0;

        //    delta = fm / *Osc::samplingRate + snareDrum_.fre[1];
        //    float sin2 = 0.08f * (float)SMPSINUS(snareDrum_.timers[1]);
        //    snareDrum_.timers[1] += delta; if (snareDrum_.timers[1] > 1.0) snareDrum_.timers[1] -= 1.0;

        //    float noise = 0.6f * (float)SMPNOISE(snareDrum_.fre[2], snareDrum_.timers[2], snareDrum_.noise);

        //    snareDrum_.filters[0].update(cut);
        //    var lp = snareDrum_.filters[0].run(noise);
        //    buffer[i] += am * (sin1 + sin2) + am * lp;
        //}
    }
}

void Drums::renderHihat(GenericDrum* hihat, float* buffer, int start, int end) {
    for (var i = start; i < end; i++) {

        float am = hihat->envelopes[0].run();
        float fm = hihat->envelopes[1].run();
        float click = hihat->envelopes[2].run();
        float cut = hihat->envelopes[3].run();
        am += click;
        cut += click;

        // create 6 pulses
        double smp = 0.0f;
        float delta;
        for (var i = 0; i < 6; i++) {
            delta = hihat->fre[i] + fm;
            smp += SMPPULSE(hihat->timers[i]);  //hihat->amp[i] * 
            hihat->timers[i] += delta;
            if (hihat->timers[i] > 1.0) hihat->timers[i] -= 1.0;
        }
        smp *= hihat->amp[0];

        hihat->filters[0].update(cut);
        hihat->filters[1].update(cut);
        var hp = hihat->filters[0].run((float)smp);
        buffer[i] += hihat->filters[1].run(am * hp);
    }
}

void Drums::renderOpenHihat(float* buffer, int start, int end) {
    if (openHihat_.isActive()) {
        renderHihat(&openHihat_, buffer, start, end);
    }
}

void Drums::renderClosedHihat(float* buffer, int start, int end) {
    if (closedHihat_.isActive()) {
        renderHihat(&closedHihat_, buffer, start, end);
    }
}

void Drums::renderLowTom(float* buffer, int start, int end) {
    if (lowTom_.isActive()) {
        lowTom_.render(buffer, start, end);
    }
}
void Drums::renderMidTom(float* buffer, int start, int end) {
    if (midTom_.isActive()) {
        midTom_.render(buffer, start, end);
    }
}
void Drums::renderHighTom(float* buffer, int start, int end) {
    if (highTom_.isActive()) {
        highTom_.render(buffer, start, end);
    }
}

void Drums::renderClap(float* buffer, int start, int end) {
    if (clap_.envelopes[0].phase() == EnvPhase::Up) {
        clap_.attribute1(1);
    }
    if (clap_.attribute1() != 0) {
        for (var i = start; i < end; i++) {

            float am = clap_.envelopes[0].run() + clap_.envelopes[1].run() + clap_.envelopes[2].run() + clap_.envelopes[3].run();

            var delta = clap_.fre[0];
            float sin1 = clap_.amp[0] * (float)SMPPULSE(clap_.timers[0]);
            clap_.timers[0] += delta; if (clap_.timers[0] > 1.0) clap_.timers[0] -= 1.0;

            delta = clap_.fre[1];
            float sin2 = clap_.amp[1] * (float)SMPPULSE(clap_.timers[1]);
            clap_.timers[1] += delta; if (clap_.timers[1] > 1.0) clap_.timers[1] -= 1.0;

            SMPNOISE(clap_.fre[2], clap_.timers[2], clap_.noise);

            var smp = sin1 + sin2 + clap_.amp[2] * clap_.noise;

            clap_.filters[0].update(0.0f);
            clap_.filters[1].update(am);
            var hp = clap_.filters[0].run((float)smp);
            var lp = clap_.filters[1].run(am * hp);
            buffer[i] += lp;
        }
        if (clap_.envelopes[3].phase() == EnvPhase::Up) {
            clap_.attribute1(2);
        }
        if (clap_.attribute1() == 2 && clap_.envelopes[3].phase() == EnvPhase::Idle) {
            clap_.attribute1(0);
        }
    }
}

void Drums::setControl(int id, S value) {
    switch (id)
    {
        //case bdTone: break;
        //case bdTune: break;
        //case bdDecay: break;
        //case sdDecay: break;
        //case sdTune: break;
        //case sdSnappy: break;
        //case sdTone: break;
        case hhTone:
            controls_.ch.flt1.cut.value.b = value.b;
            controls_.oh.flt1.cut.value.b = value.b;
            value.b += value.b;
            controls_.ch.flt2.cut.value.b = value.b;
            controls_.oh.flt2.cut.value.b = value.b; 
            break;
        //case ohDecay: break;
        default:
            pControls_[id].value = value; break;
    }
}

void Drums::run(int start, int end) {
    memset(outputs_[0], 0, sizeof(float) * (end - start));
    renderBassDrum(outputs_[0], start, end);
    renderSnareDrum(outputs_[0], start, end);
    renderOpenHihat(outputs_[0], start, end);
    renderClosedHihat(outputs_[0], start, end);
    //renderLowTom(outputs_[0], start, end);
    //renderMidTom(outputs_[0], start, end);
    //renderHighTom(outputs_[0], start, end);
    //renderClap(outputs_[0], start, end);
}
void Drums::setNote(byte note, byte velocity) {
    // C1:BD  D1:SD  E1:LT  F1:MT  G1:HT  A1:RS  H1:CL
    // C2:OH  D2:CH  E2:--  F2:--  G2:--  A2:CY  H2:CB
    GenericDrum* drum = &bassDrum_;
    switch (note) {
    case drBD:
        break;
    case drSN:
        drum = &snareDrum_;
        break;
    case drOH:
        drum = &openHihat_;
        //if (closedHihat_.isActive()) closedHihat_.setGate(0);
        break;
    case drCH:
        drum = &closedHihat_;
        //if (openHihat_.isActive()) openHihat_.setGate(0);
        break;
    case drLT:
        drum = &lowTom_;
        break;
    case drMT:
        drum = &midTom_;
        break;
    case drHT:
        drum = &highTom_;
        break;
    case drCP:
        drum = &clap_;
        break;
    }

    drum->setGate(velocity);
}
