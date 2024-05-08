#include "base/memory.h"
#include <math.h>
#include "synth/src/module/distort.h"

NS_FW_BASE_USE

using namespace SYNTH;

Distort::Distort() : Module() {
    clp_.values(&values_.clp);
    flt_.createStages(2);
    flt_.values(&values_.flt);
    createOutputBuffers(1);
}

Distort::~Distort() {

}

Value* Distort::getValues() {
    return (Value*)&values_;
}

float* Distort::getOutput(int id) {
    return outputs_[0];
}

void Distort::run(int start, int end, BufferWriteModes mode) {
    // out = clip(amp*flt(in, tone, contour), lvl))
    flt_.update(0.0f);
    for (var i = start; i < end; i++) {
        var smp = flt_.run(inputs_[0][i]);
        outputs_[0][i] = clp_.run(smp);
    }
}
