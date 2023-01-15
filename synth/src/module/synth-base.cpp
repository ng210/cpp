#include "synth/src/module/synth-base.h"

using namespace SYNTH;

SynthBase::SynthBase() {
    voiceCount_ = 1;
    setupVoice = NULL;
    renderVoice = NULL;
    freeVoice = NULL;
    setNoteVoice = NULL;
    samplingRate_ = NULL;
}

SynthBase::~SynthBase() {
    for (var i = 0; i < voiceCount_; i++) {
        (this->*freeVoice)(voices_[i]);
    }
}

void SynthBase::initialize(float* pSmpRate, int count) {
    samplingRate(pSmpRate);
    voiceCount(count);
}
void SynthBase::samplingRate(float* pSmpRate) {
    samplingRate_ = pSmpRate;
    Osc::initialize(samplingRate_);
    Lfo::initialize(samplingRate_);
    Env::initialize(*samplingRate_);
    Flt::initialize(*samplingRate_);

    for (var i = 0; i < voiceCount_; i++) {
        Voice& v = voices_[i];
        (this->*setupVoice)(v);
    }
}
void SynthBase::setNote(byte note, byte velocity) {
    if (velocity != 0) {
        // get free voice
        var candidate = &voices_[0];
        for (int i = 0; i < voiceCount_; i++) {
            Voice& voice = voices_[i];
            if (candidate->envelopes[0].ticks() < voice.envelopes[0].ticks()) {
                candidate = &voice;
            }
            if (!voice.envelopes[0].isActive()) {
                candidate = &voice;
                break;
            }
        }
        (this->*setNoteVoice)(*candidate, note, velocity);
    }
    else {
        for (int i = 0; i < voiceCount_; i++) {
            Voice& voice = voices_[i];
            if (voice.envelopes[0].phase() < EnvPhase::Down && voice.note.value.b == note) {
                (this->*setNoteVoice)(voice, note, 0);
                break;
            }
        }
    }
}
void SynthBase::voiceCount(int count) {
    var oldCount = voiceCount_;
    if (oldCount < count) {
        for (var i = oldCount; i < count; i++) {
            (this->*setupVoice)(voices_[i]);
        }
    }
    voiceCount_ = count;
}

// Module
void SynthBase::connectInput(int id, float* buffer) {
    if (id == 0) inputs_[0] = buffer;
}
float* SynthBase::getOutput(int id) {
    return outputs_[0];
}
void SynthBase::initialize(byte** pData) {
}
void SynthBase::run(int start, int end) {
    if (isActive()) {
        memset(outputs_[0], 0, sizeof(float) * (end - start));
        for (int j = 0; j < voiceCount_; j++) {
            Voice& voice = voices_[j];
            if (voice.envelopes[0].isActive()) {
                (this->*renderVoice)(voice, outputs_[0], start, end);
            }
        }
    }
}