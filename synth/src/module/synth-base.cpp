#include "synth/src/module/synth-base.h"
#include "base/memory.h"
#include "base/debug.h"

//#define SETNOTEDEBUG

using namespace SYNTH;

SynthBase::SynthBase() : Module() {
    voiceCount_ = 0;
    setupVoiceHandler = NULL;
    setNoteVoiceHandler = Voice::setNoteVoiceDefaultHandler;
    freeVoiceHandler = Voice::freeVoiceDefaultHandler;
    killVoiceHandler = Voice::killVoiceDefaultHandler;
    renderVoiceHandler = NULL;
    //samplingRate_ = NULL;
}

SynthBase::~SynthBase() {
    for (var vi = 0; vi < 2*voiceCount_; vi++) {
        freeVoiceHandler(&voices_[vi]);
    }
}

void SynthBase::initialize(int count) {
    //samplingRate(pSmpRate);
    voiceCount(count);
}

#pragma region Voice handling
void SynthBase::voiceCount(int count) {
    var oldCount = voiceCount_;

    // set up new voices
    for (var vi = 0; vi < count; vi++) {
        var vu = &voiceUnits_[vi];
        var v1 = &voices_[vi];
        var v2 = &voices_[vi + count];
        vu->setVoices(v1, v2);
        // setup voice
        if (v1->envelopes == NULL) {
            v1->values = getValues();
            setupVoiceHandler(v1, getValues());
        }
        if (v2->envelopes == NULL) {
            v2->values = getValues();
            setupVoiceHandler(v2, getValues());
        }
    }

    // free unused voices
    for (var vi = 2*count; vi < 2*oldCount; vi++) {
        freeVoiceHandler(&voices_[vi]);
    }

    voiceCount_ = count;
}

void SynthBase::setNote(byte note, byte velocity) {
    if (velocity != 0) {
        // get free voice
        var vi = 0;
        VoiceUnit* candidate = &voiceUnits_[vi];
        for (; vi < voiceCount_; vi++) {
            // first inactive
            VoiceUnit* vu = &voiceUnits_[vi];
            if (!vu->isActive()) {
                candidate = vu;
                break;
            }
            // the longest running
            if (candidate->ticks() < vu->ticks()) {
                candidate = vu;
            }
        }

        killVoiceHandler(candidate->activeVoice());
        setNoteVoiceHandler(candidate->swapVoices(), note, velocity);
#ifdef SETNOTEDEBUG
        printf(" ON %d, %d\n", note, velocity);
        var id = (word)((long long)candidate & 0xffff);
        printf(" - on %X.%d\n", id, candidate->activeVoiceId());
        printf(" - kill %X.%d\n", id, 1 - candidate->activeVoiceId());
#endif
    }
    else {
#ifdef SETNOTEDEBUG
        printf(" OFF %d\n", note);
#endif
        for (int vi = 0; vi < voiceCount_; vi++) {
            VoiceUnit* vu = &voiceUnits_[vi];
            if (vu->matchVoiceForNoteOff(note)) {
                setNoteVoiceHandler(vu->activeVoice(), note, 0);
#ifdef SETNOTEDEBUG
                var id = (word)((long long)vu & 0xffff);
                printf(" - off %X.%d\n", id, vu->activeVoiceId());
#endif
                break;
            }
        }
    }
}

#pragma endregion

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
        //var isActive = false;
        for (int j = 0; j < 2 * voiceCount_; j++) {
            Voice* voice = &voices_[j];
            if (voice->envelopes[0].isActive()) {
                renderVoiceHandler(voice, this, outputs_[0], start, end);
                //isActive = true;
            }
        }
        //isActive_ = isActive;
    }
}