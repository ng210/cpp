#include "math.h"
#include "math.h"
#include "base/memory.h"
#include "utils/utils.h"
#include "synth/src/module/drums.h"
#include "synth/src/module/synth-def.h"

NS_FW_BASE_USE
using namespace SYNTH;

Drums::Drums() : Module() {
    // 4 stereo input buffers
    // stereo output buffer
    createOutputBuffers(2*(4 + 1));
    memset(&stages_, 0, 4 * sizeof(DrumStage));
}

Drums::~Drums() {
}

Value* Drums::getValues() {
    return (Value*)&values_;
}

bool Drums::isActive() {
    var isActive = true;
    for (var di = 0; di < DRUMS_COUNT; di++) {
        isActive = isActive && drums_[di].isActive();
    }
    return isActive;
}

void Drums::connectEffect(Module* effect) {
    for (var si = 0; si < 4; si++) {
        if (stages_[si].effect == NULL) {
            stages_[si].effect = effect;
            stages_[si].buffer[0] = outputs_[2 + 2*si];
            stages_[si].buffer[1] = outputs_[3 + 2*si];
            effect->connectInput(0, stages_[si].buffer[0]);
            effect->connectInput(1, stages_[si].buffer[1]);
            break;
        }
    }
}

void Drums::run(int start, int end, BufferWriteModes mode) {
    // clear input buffers and output
    memset(outputs_[0], 0, 2 * (4 + 1) * SAMPLE_BUFFER_SIZE);

    // for each drum
    for (var di = 0; di < DRUMS_COUNT; di++) {
        GenericDrum& drum = drums_[di];
        if (drum.isActive()) {
            // render drum
            drum.run(start, end);

            // mix drum with stereo panning into output
            var pan = values_.drums[di].pan.f;
            for (var i = start; i < end; i++) {
                var output = drum.getOutput(0)[i];
                outputs_[0][i] += (1.0f - pan) * output;
                outputs_[1][i] += pan * output;
    
                // mix mono output with send into stage input
                for (var si = 0; si < 4; si++) {
                    if (stages_[si].effect != NULL) {
                        var send = values_.drums[di].send[si].f * output;
                        stages_[si].buffer[0][i] += send;
                        stages_[si].buffer[1][i] += send;
                    }
                }
            }
        }
    }
    
    // run and mix stage into output
    for (var si = 0; si < 4; si++) {
        if (stages_[si].effect != NULL) {
            stages_[si].effect->run(start, end);
            for (var i = start; i < end; i++) {
                outputs_[0][i] += stages_[si].effect->getOutput(0)[i];
                outputs_[1][i] += stages_[si].effect->getOutput(1)[i];
            }
        }
    }
}

void Drums::setNote(byte note, byte velocity) {
    // C1:BD  D1:SD  E1:LT  F1:MT  G1:HT  A1:RS  H1:CL
    // C2:OH  D2:CH  E2:--  F2:--  G2:--  A2:CY  H2:CB
    var ix = (note>>1) & 0x07;
    drums_[ix].setNote(note, velocity);
}
