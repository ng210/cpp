#include "synth/src/module/mixer.h"
#include "base/memory.h"

using namespace SYNTH;

int Mixer8x4::frame_ = 0;
float Mixer8x4::buffer_[SAMPLE_BUFFER_SIZE];

Mixer8x4::Mixer8x4() : Module() {
    //combinedBuffer_ = MALLOC(float, 8 * 4 * 1024);
    //var buffer = combinedBuffer_;
    for (var ci = 0; ci < 8; ci++) {
        var chn = &channels_[ci];
        chn->input = NULL;
        chn->stageCount = 0;
        //chn->controls = &controls.channels[i];
        //chn->values->controls->amp.init(0, 255, 1, 80);
        //chn->values->controls->pan.init(0, 255, 1, 127);
        //chn->values->controls->gain.init(0, 255, 1, 100);
        ////channel->buffer = buffer;
        ////buffer += 1024;
        for (var si = 0; si < 4; si++) {
            var st = &chn->stages[si];
            st->effect = NULL;
            //stage->controls = &channel->controls->stages[si];
            ////stage->controls->blend.init(0.0f, 1.0f, 0.01f, 0.0f);
            //stage->controls->gain.init(0, 255, 1, 100);
            //stage->buffer = buffer;
            //buffer += 1024;
        }
    }
    channelCount_ = 0;
    isMono_ = false;
}

Mixer8x4::~Mixer8x4() {
    //FREE(combinedBuffer_);
}

Value* Mixer8x4::getValues() {
    return (Value*)&values_;
}

//void Mixer8x4::initializeFromStream(byte** pData) {
//    channelCount_ = READ(*pData, byte);
//    if (channelCount_ >= 8) channelCount_ = 8;
//    for (var ci = 0; ci < channelCount_; ci++) {
//        var channel = &channels_[ci];
//        channel->controls->amp.setFromStream(*pData);
//        channel->controls->pan.setFromStream(*pData);
//        channel->controls->gain.setFromStream(*pData);
//        channel->stageCount = READ(*pData, byte);
//        if (channel->stageCount >= 4) channel->stageCount = 4;
//        for (var si = 0; si < channel->stageCount; si++) {
//            var stage = &channel->stages[si];
//            //stage->controls->blend.setFromStream(*pData);
//            stage->controls->gain.setFromStream(*pData);
//        }
//    }
//}

MixerChannel* Mixer8x4::connectInput(MixerChannel* channel, Module* input) {
    if (channel != NULL) {
        channel->input = input;
    }
    return channel;
}

MixerChannel* Mixer8x4::connectInput(int chnId, Module* input, float amp, byte pan, byte gain) {
    var chn = setupChannel(chnId, amp, pan, gain);
    if (chn != NULL) chn->input = input;
    return chn;
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

MixerChannel* Mixer8x4::setupChannel(int chnId, float amp, byte pan, byte gain) {
    var chn = getChannel(chnId);
    if (chn != NULL) {
        values_.channels[chnId].amp = amp;
        values_.channels[chnId].pan = pan / 255.0f;
        values_.channels[chnId].gain = gain / 255.0f;
    }
    return chn;
}

//void Mixer8x4::run(float* buffer, int start, int end) {
//    // currently unused   
//}

void Mixer8x4::fillSoundBuffer(short* buffer, int sampleCount, void* args) {
    // [input] ─┬→*gain────────────────────────────→(++++)─→(pan,*amp)─→ [output]
    //          │                                    ↑↑↑↑
    //          └→stage1┬→*gain──────────────────────┘│││
    //                  └→stage2┬→*gain───────────────┘││
    //                          └→stage3┬→*gain────────┘│
    //                                  └→stage4─→*gain─┘
    // output = amp * pan(input * gain
    //                  + stage1.effect * gain
    //                  + stage2.effect * gain
    //                  + stage3.effect * gain
    //                  + stage4.effect * gain)
    var mixer = (Mixer8x4*)args;

    // run inputs and stages
    for (var ci = 0; ci < mixer->channelCount_; ci++) {
        var chn = &mixer->channels_[ci];
        chn->input->run(0, sampleCount);
        for (var i = 0; i < chn->stageCount; i++) {
            if (chn->stages[i].effect != NULL) {
                chn->stages[i].effect->run(0, sampleCount);
            }
        }
    }

    int j = 0;
    for (var i = 0; i < sampleCount; i++) {
        int left = 0, right = 0;
        for (var ci = 0; ci < mixer->channelCount_; ci++) {
            float chLeft, chRight;
            MixerChannel& ch = mixer->channels_[ci];
            var gain = mixer->values_.channels[ci].gain.f;
            // output = input * gain
            chLeft = gain * ch.input->getOutput(0)[i];
            chRight = ch.input->isMono() ? chLeft : gain * ch.input->getOutput(1)[i];

            for (var si = 0; si < ch.stageCount; si++) {
                MixerStage& st = ch.stages[si];
                if (st.effect != NULL) {
                    gain = mixer->values_.channels[ci].stages[si].gain.f;
                    var v = gain * st.effect->getOutput(0)[i];
                    chLeft += v;
                    chRight += st.effect->isMono() ? v : gain * st.effect->getOutput(1)[i];
                }
            }

            // panning clipping and final amplification
            var amp = 32768.0f * mixer->values_.channels[ci].amp.f;
            var pan = 2.0f * mixer->values_.channels[ci].pan.f;
            //chLeft *= amp * (2.0f - pan);
            //chRight *= amp * pan;
            //left += chLeft;
            //right += chRight;

            left += (int)(amp * (2.0f - pan) * chLeft);
            right += (int)(amp * pan * chRight);
        }
        if (left > 32767) left = 32767; else if (left < -32767) left = -32767;
        if (right > 32767) right = 32767; else if (right < -32767) right = -32767;
        buffer[j++] += (short)left;
        buffer[j++] += (short)right;
    }
}
