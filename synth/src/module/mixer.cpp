#include "mixer.h"
#include "base/memory.h"


using namespace SYNTH;

int Mixer8x4::frame_ = 0;
float Mixer8x4::buffer_[SAMPLE_BUFFER_SIZE];

Soundbank* Mixer8x4::defaultSoundbank_;

Mixer8x4::Mixer8x4() : Module((PotBase*)&controls, MixerCtrlsCount) {
    //combinedBuffer_ = MALLOC(float, 8 * 4 * 1024);
    //var buffer = combinedBuffer_;
    for (var i = 0; i < 8; i++) {
        var channel = &channels_[i];
        channel->controls = &controls.channels[i];
        channel->controls->amp.init(0, 255, 1, 80);
        channel->controls->pan.init(0, 255, 1, 127);
        channel->controls->gain.init(0, 255, 1, 100);
        //channel->buffer = buffer;
        //buffer += 1024;
        for (var si = 0; si < 4; si++) {
            var stage = &channel->stages[si];
            stage->controls = &channel->controls->stages[si];
            //stage->controls->blend.init(0.0f, 1.0f, 0.01f, 0.0f);
            stage->controls->gain.init(0, 255, 1, 100);
            stage->effect = NULL;
            //stage->buffer = buffer;
            //buffer += 1024;
        }
        channel->input = NULL;
        channel->stageCount = 0;
    }
    channelCount_ = 0;
    isMono_ = false;
}

Mixer8x4::~Mixer8x4() {
    //FREE(combinedBuffer_);
}

void Mixer8x4::initializeFromStream(byte** pData) {
    channelCount_ = READ(*pData, byte);
    if (channelCount_ >= 8) channelCount_ = 8;
    for (var ci = 0; ci < channelCount_; ci++) {
        var channel = &channels_[ci];
        channel->controls->amp.setFromStream(*pData);
        channel->controls->pan.setFromStream(*pData);
        channel->controls->gain.setFromStream(*pData);
        channel->stageCount = READ(*pData, byte);
        if (channel->stageCount >= 4) channel->stageCount = 4;
        for (var si = 0; si < channel->stageCount; si++) {
            var stage = &channel->stages[si];
            //stage->controls->blend.setFromStream(*pData);
            stage->controls->gain.setFromStream(*pData);
        }
    }
}

MixerChannel* Mixer8x4::connectInput(MixerChannel* channel, Module* input) {
    if (channel != NULL) {
        channel->input = input;
    }
    return channel;
}

void Mixer8x4::run(float* buffer, int start, int end) {
    // currently unused   
}

MixerChannel* Mixer8x4::connectEffect(MixerChannel* channel, Module* effect, int stageId) {
    if (channel != NULL) {
        if (stageId == -1) {
            stageId = channel->stageCount;
            channel->stageCount++;
        }
        if (stageId < 4) {
            var input = stageId == 0 ? channel->input : channel->stages[stageId - 1].effect;
            // try to connect the effect as stereo:
            // read 2 outputs and connect them to 2 inputs
            for (var i = 0; i < 2; i++) {
                effect->connectInput(i, input->getOutput(i));
            }
            channel->stages[stageId].effect = effect;
        }
    }
    return channel;
}

MixerChannel* Mixer8x4::getChannel(int id) {
    return id < channelCount_ ? &channels_[id] : NULL;
}

void Mixer8x4::fillSoundBuffer(short* buffer, int sampleCount, void* args) {
    var mixer = (Mixer8x4*)args;
    //memset(mixer->combinedBuffer_, 0, 8 * 4 * 1024 * sizeof(float));

    // run inputs and stages
    for (var ci = 0; ci < mixer->channelCount(); ci++) {
        var ch = &mixer->channels_[ci];
        ch->input->run(0, sampleCount);
        for (var i = 0; i < ch->stageCount; i++) {
            if (ch->stages[i].effect != NULL) {
                ch->stages[i].effect->run(0, sampleCount);
            }
        }
    }

    int j = 0;
    for (var i = 0; i < sampleCount; i++) {
        float left = 0.0f, right = 0.0f;
        for (var ci = 0; ci < mixer->channelCount(); ci++) {
            float chLeft, chRight;
            var ch = &mixer->channels_[ci];
            if (ch->input->isMono()) {
                chLeft = chRight = ch->controls->gain.value.f * ch->input->getOutput(0)[i];
            }
            else {
                chLeft = ch->controls->gain.value.f * ch->input->getOutput(0)[i];
                chRight = ch->controls->gain.value.f * ch->input->getOutput(1)[i];
            }

            for (var si = 0; si < ch->stageCount; si++) {
                var st = ch->stages[si];
                if (st.effect != NULL) {
                    if (st.effect->isMono()) {
                        var stOut = st.effect->getOutput(0)[i] * st.controls->gain.value.f;
                        chLeft += stOut;
                        chRight += stOut;
                    }
                    else {
                        chLeft += st.effect->getOutput(0)[i] * st.controls->gain.value.f;
                        chRight += st.effect->getOutput(1)[i] * st.controls->gain.value.f;
                    }
                }
            }

            // panning clipping and final amplification
            if (chLeft > 1.0f) chLeft = 1.0f; else if (chLeft < -1.0f) chLeft = -1.0f;
            if (chRight > 1.0f) chRight = 1.0f; else if (chRight < -1.0f) chRight = -1.0f;
            var amp = 32768.0f * ch->controls->amp.value.f;
            var pan = 2.0f * ch->controls->pan.value.f;
            chLeft *= amp * (2.0f - pan);
            chRight *= amp * pan;
            left += chLeft;
            right += chRight;
        }
        buffer[j++] += (short)left;
        buffer[j++] += (short)right;
    }

    //for (var ci = 0; ci < mixer->channelCount(); ci++) {
    //    var ch = &mixer->channels_[ci];
    //    var j = 0;
    //    for (var i = 0; i < sampleCount; i++) {
    //        // smp = gain * input
    //        float left = 0.0f;
    //        float right = 0.0f;
    //        if (ch->input->isMono()) {
    //            left = right = ch->controls->gain.value.f * ch->input->getOutput(0)[i];
    //        }
    //        else {
    //            left = ch->controls->gain.value.f * ch->input->getOutput(0)[i];
    //            right = ch->controls->gain.value.f * ch->input->getOutput(1)[i];
    //        }

    //        for (var si = 0; si < ch->stageCount; si++) {
    //            var st = ch->stages[si];
    //            if (st.effect != NULL) {
    //                // smp += gain * stage
    //                if (ch->input->isMono()) {
    //                    var smp = st.controls->gain.value.f * st.effect->getOutput(0)[i];
    //                    left += smp;
    //                    right += smp;
    //                }
    //                else {
    //                    left += st.controls->gain.value.f * st.effect->getOutput(0)[i];
    //                    right += st.controls->gain.value.f * st.effect->getOutput(1)[i];
    //                }
    //            }
    //        }

    //        // amp * smp
    //        var amp = 32768.0f * ch->controls->amp.value.f;
    //        left *= 32768.0f * amp * (1.0f - ch->controls->pan.value.f);
    //        right *= 32768.0f * amp * ch->controls->pan.value.f;
    //        buffer[j++] += (short)left;
    //        buffer[j++] += (short)right;
    //    }
    //}
}

Soundbank* Mixer8x4::getDefaultSoundbank() {
    return Mixer8x4::defaultSoundbank_;
}
