#include "synth.h"
#include "voice.h"
#include "base/memory.h"

NS_FW_BASE_USE
using namespace SYNTH;

Synth::Synth(float* smpRate, int voiceCount) {
    pControls_ = (PotBase**)&controls_;
    voices_.init(sizeof(Voice), 32);
    initialize(smpRate, voiceCount);
    controls_.amp.value = 1.0f;
    program_ = -1;
    createOutputBuffers(1);
    isMono_ = true;
}

Synth::~Synth() {
    var i = voices_.length();
    while (i-- > 0) {
        ((Voice*)voices_.getAt(0))->freeResources();
        voices_.removeAt(0);
    }
}

#pragma region Module
void Synth::initialize(byte** pData) {

}

void Synth::connectInput(int id, float* buffer) {
    if (id == 0) inputs_[0] = buffer;
}

float* Synth::getOutput(int id) {
    return outputs_[0];
}

void Synth::run(int start, int end) {
    if (isActive) {
        for (var i = start; i < end; i++) {
            var smp = 0.0f;
            for (int j = 0; j < voices_.length(); j++) {
                var voice = (Voice*)voices_.getAt(j);
                if (voice->isActive()) {
                    smp += voice->run();
                }
            }
            outputs_[0][i] = smp * controls_.amp.value.f;
        }
    }
}
#pragma endregion

#pragma region Synth
void Synth::initialize(float* smpRate, int voiceCount) {
    samplingRate(smpRate);
    setVoiceCount(voiceCount);
}

void Synth::samplingRate(float* value) {
    samplingRate_ = *value;
    omega = SYNTH_THETA / (double)samplingRate_;
    voices_.forEach([](void* p, UINT32 ix, Collection* array, void* samplingRate) {
        ((Voice*)p)->setSamplingRate((float*)samplingRate);
        return 1;
    }, &samplingRate_);
    Env::initialize(samplingRate_);
    Flt::initialize(samplingRate_);

}

void Synth::setVoiceCount(int voiceCount) {
    var count = voiceCount;
    var oldCount = voices_.length();
    if (oldCount < count) {
        var pots = (Pot**)&controls_;
        Voice voice;
        for (var i=oldCount; i< count; i++) {
            var vp = (Voice*)voices_.add(&voice); new (vp) Voice();
            vp->envelopes[0].assignControls((Pot*)&controls_.env1);
            vp->envelopes[1].assignControls((Pot*)&controls_.env2);
            vp->envelopes[2].assignControls((Pot*)&controls_.env3);
            vp->envelopes[3].assignControls((Pot*)&controls_.env4);
            vp->lfos[0].assignControls((Pot*)&controls_.lfo1);
            vp->lfos[1].assignControls((Pot*)&controls_.lfo2);
            vp->oscillators[0].assignControls((Pot*)&controls_.osc1);
            vp->oscillators[1].assignControls((Pot*)&controls_.osc2);
            vp->filters[0].assignControls((Pot*)&controls_.flt1);
        }
    } else {
        for (var i = oldCount; i > count; --i) {
            voices_.removeAt(i);
        }
    }
}

void Synth::setNote(byte note, byte velocity) {
    if (velocity != 0) {
        // get free voice
        var candidate = (Voice*)voices_.getAt(0);
        for (int i=0; i<voices_.length(); i++) {
            var voice = (Voice*)voices_.getAt(i);
            if (candidate->getTicks() < voice->getTicks()) {
                candidate = voice;
            }
            if (!voice->isActive()) {
                candidate = voice;
                break;
            }
        }
        candidate->setNote(note, velocity);
    } else {
        for (int i=0; i<voices_.length(); i++) {
            var voice = (Voice*)voices_.getAt(i);
            if (voice->envelopes[0].phase() < EnvPhase::Down && voice->note().value.b == note) {
                voice->setNote(note, 0);
                break;
            }
        }
    }
}

void Synth::setProgram(int prgId) {
    var sb = soundBank_;
    program_ = prgId;
    isActive = true;
    var count = (int)*sb;
    if (prgId < count) {
        var offset = *(short*)(sb + 1 + 16 * prgId + 14);
        sb += offset;
        count = *sb++;
        for (var i = 0; i < count; i++) {
            var iid = *sb++;
            var pot = getControl(iid);
            pot->setFromStream(sb);
        }
    }
}

#pragma endregion