#include "synth/src/module/synth-base.h"

using namespace SYNTH;

SynthBase::SynthBase() : Module() {
    voiceCount_ = 0;
    setupVoice = NULL;
    renderVoice = NULL;
    freeVoice = NULL;
    setNoteVoice = NULL;
    //samplingRate_ = NULL;
}

SynthBase::~SynthBase() {
    for (var i = 0; i < voiceCount_; i++) {
        (this->*freeVoice)(voices_[i]);
    }
}

void SynthBase::initialize(int count) {
    //samplingRate(pSmpRate);
    voiceCount(count);
}

void SynthBase::setNote(byte note, byte velocity) {
    if (velocity != 0) {
        //printf(" ** ON\n");
        // get free voice
        var vi = 0;
        Voice* candidate = &voices_[vi];
        for (; vi < voiceCount_; vi++) {
            // first inactive
            Voice* voice = &voices_[vi];
            //printf("   - %d,%d,%d,%d\n", vi, voice->envelopes[0].phase(), note, velocity);
            if (!voice->envelopes[0].isActive()) {
                candidate = voice;
                break;
            }
            // the longest running
            if (candidate->envelopes[0].ticks() < voice->envelopes[0].ticks()) {
                candidate = voice;
            }
        }
        //printf(" - %d,%d,%d,%d\n", vi, candidate->envelopes[0].phase(), note, velocity);
        (this->*setNoteVoice)(*candidate, note, velocity);
    }
    else {
        //printf(" ** OFF\n");
        for (int i = 0; i < voiceCount_; i++) {
            Voice* voice = &voices_[i];
            //printf("   - %d,%d,%d,%d\n", i, voice->envelopes[0].phase(), note, velocity);
            if (voice->envelopes[0].phase() < EnvPhase::Down && voice->note.b == note) {
                (this->*setNoteVoice)(*voice, note, 0);
                //printf(" - %d,%d,%d,%d\n", i, voice->envelopes[0].phase(), note, velocity);
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
    else {
        for (var i = count; i < oldCount; i++) {
            (this->*freeVoice)(voices_[i]);
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
bool SynthBase::isActive() {
    //var isActive = false;
    //for (var i = 0; i < voiceCount_; i++) {
    //    isActive |= voices_[i].envelopes[0].isActive();
    //}
    return isActive_;
}
void SynthBase::run(int start, int end) {
    if (isActive()) {
        memset(outputs_[0], 0, sizeof(float) * (size_t)(end - start));
        for (int j = 0; j < voiceCount_; j++) {
            Voice& voice = voices_[j];
            if (voice.envelopes[0].isActive()) {
                (this->*renderVoice)(voice, outputs_[0], start, end);
            }
        }
    }
}