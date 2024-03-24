#include "synth/src/module/module.h"
#include "synth/src/module/voice.h"
#include "base/memory.h"

//NS_FW_BASE_USE
using namespace SYNTH;

#pragma region Voice
Voice::Voice() {
    envCount = 0;
    envelopes = NULL;
    lfoCount = 0;
    lfos = NULL;
    oscCount = 0;
    oscillators = NULL;
    fltCount = 0;
    filters = NULL;
}

void Voice::setNoteVoiceDefaultHandler(Voice* v, byte note, byte velocity) {
    v->velocity = velocity / 255.0f;
    v->note = note;
    if (velocity != 0) {
        for (var li = 0; li < v->lfoCount; li++) v->lfos[li].reset();
        for (var oi = 0; oi < v->oscCount; oi++) {
            v->oscillators[oi].reset();
        }
    }


    for (var ei = 0; ei < v->envCount; ei++) {
        v->envelopes[ei].setGate(velocity);
    }
}
void Voice::freeVoiceDefaultHandler(Voice* v) {
    if (v->envCount > 0) DELARR(v->envelopes);
    if (v->lfoCount > 0) DELARR(v->lfos);
    if (v->oscCount > 0) DELARR(v->oscillators);
    if (v->fltCount > 0) DELARR(v->filters);
}
void Voice::killVoiceDefaultHandler(Voice* v) {
    for (var ei = 0; ei < v->envCount; ei++) {
        v->envelopes[ei].kill();
    }
}

void Voice::setupVoiceAdsr(int count, AdsrValues* values) {
    envCount = count;
    if (count > 0) {
        envelopes = NEWARR(Adsr, count);
        for (var ei = 0; ei < count; ei++) {
            ((Adsr*)envelopes)[ei].setValues((Value*)values);
            *(byte**)&values = (byte*)values + sizeof(AdsrValues);
        }
    }
}
void Voice::setupVoiceDahr(int count, DahrValues* values) {
    envCount = count;
    if (count > 0) {
        envelopes = NEWARR(Dahr, count);
        for (var ei = 0; ei < count; ei++) {
            ((Dahr*)envelopes)[ei].setValues((Value*)values);
            *(byte**)&values = (byte*)values + sizeof(DahrValues);
        }
    }
}
void Voice::setupVoiceLfo(int count, LfoValues* values) {
    lfoCount = count;
    lfos = NEWARR(Lfo, count);
    for (var li = 0; li < count; li++) {
        lfos[li].values((LfoValues*)values);
        *(byte**)&values = (byte*)values + sizeof(LfoValues);
    }
}
void Voice::setupVoiceOsc(int count, OscValues* values) {
    oscCount = count;
    oscillators = NEWARR(Osc, count);
    for (var oi = 0; oi < count; oi++) {
        oscillators[oi].setNoteControl(&note);
        oscillators[oi].values((OscValues*)values);
        *(byte**)&values = (byte*)values + sizeof(OscValues);
    }
}
void Voice::setupVoiceFlt(int count, FltValues* values) {
    fltCount = count;
    filters = NEWARR(Flt, count);
    for (var fi = 0; fi < count; fi++) {
        filters[fi].createStages(2);
        filters[fi].values((FltValues*)values);
        *(byte**)&values = (byte*)values + sizeof(FltValues);
    }
}
#pragma endregion

#pragma region VoiceUnit
VoiceUnit::VoiceUnit() {
    activeVoiceId_ = 0;
    voices_[0] = NULL;
    voices_[1] = NULL;
}

VoiceUnit::~VoiceUnit() {
}

void VoiceUnit::setVoices(Voice* a, Voice* b) {
    voices_[0] = a;
    voices_[1] = b;
}

Voice* VoiceUnit::activeVoice() {
    return voices_[activeVoiceId_];
}

bool VoiceUnit::isActive() {
    return activeVoice()->envelopes[0].phase() != EnvPhase::Idle;
}

Voice* VoiceUnit::swapVoices() {
    activeVoiceId_ = 1 - activeVoiceId_;
    return activeVoice();
}
bool VoiceUnit::matchVoiceForNoteOff(byte note) {
    return activeVoice()->envelopes[0].phase() < EnvPhase::Down && activeVoice()->note.b == note;
}

int VoiceUnit::ticks() {
    return activeVoice()->envelopes[0].ticks();
}
#pragma endregion