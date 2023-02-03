#include "base/memory.h"
#include "math.h"
#include "distort.h"

NS_FW_BASE_USE

using namespace SYNTH;

Distort::Distort(float* samplingRate) {
    pControls_ = (PotBase*)&controls_;
    controls_.mod.value.f = 0.0f;
    clp_.assignControls(&controls_.amp);
    flt_.createStages(3);
    flt_.assignControls(&controls_.cut);
    createOutputBuffers(1);
    updateFilter();
}

Distort::~Distort() {

}

void Distort::initialize(byte** pData) {
    if (pData != NULL && *pData != NULL) {
        controls_.amp.setFromStream(*pData);
        controls_.lvl.setFromStream(*pData);
        controls_.cut.setFromStream(*pData);	// tone
        controls_.res.setFromStream(*pData);	// contour
        controls_.mod.value.b = 255;
        controls_.mode.setFromStream(*pData);	// mode
        updateFilter();
    }
}

float* Distort::getOutput(int id) {
    return outputs_[0];
}

void Distort::run(int start, int end) {
    // out = flt(clip(amp*in, lvl), tone, contour)
    for (var i = start; i < end; i++) {
        var smp = clp_.run(inputs_[0][i] * controls_.amp.value.f);
        smp = flt_.run(smp);
        outputs_[0][i] = smp;
    }
}

void Distort::updateFilter() {
    flt_.update(0.0f);
}