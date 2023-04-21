#include "base/memory.h"
#include "math.h"
#include "distort.h"

NS_FW_BASE_USE

using namespace SYNTH;

Soundbank* Distort::defaultSoundbank_ = NULL;

Distort::Distort(float* samplingRate) : Module((PotBase*)&controls, DistortCtrlsCount) {
    controls.mod.value.f = 0.0f;
    clp_.assignControls(&controls.amp);
    flt_.createStages(3);
    flt_.assignControls(&controls.cut);
    updateFilter();
    program_ = 0;
    createOutputBuffers(1);
    setSoundbank(getDefaultSoundbank());
}

Distort::~Distort() {

}

//void Distort::initializeFromStream(byte** pData) {
//    if (pData != NULL && *pData != NULL) {
//        controls.amp.setFromStream(*pData);
//        controls.lvl.setFromStream(*pData);
//        controls.cut.setFromStream(*pData);	// tone
//        controls.res.setFromStream(*pData);	// contour
//        controls.mod.value.b = 255;
//        controls.mode.setFromStream(*pData);	// mode
//        updateFilter();
//    }
//}

float* Distort::getOutput(int id) {
    return outputs_[0];
}

void Distort::run(int start, int end) {
    updateFilter();
    // out = flt(clip(amp*in, lvl), tone, contour)
    for (var i = start; i < end; i++) {
        var smp = clp_.run(inputs_[0][i] * controls.amp.value.f);
        smp = flt_.run(smp);
        outputs_[0][i] = smp;
    }
}

void Distort::updateFilter() {
    flt_.update(0.0f);
}

Soundbank* Distort::getDefaultSoundbank() {
    return Distort::defaultSoundbank_;
}

void Distort::prepare() {
    var bassBoost = NEW_(Stream);
    {
        // amp, lvl
        bassBoost->writeFloat(2.0f)->writeByte(240);
        // cut, res, mod, mode
        bassBoost->writeByte(40)->writeByte(80)->writeByte(0)->writeByte(FmLowPass);
    };

    var grunge = NEW_(Stream);
    {
        // amp, lvl
        grunge->writeFloat(6.0f)->writeByte(100);
        // cut, res, mod, mode
        grunge->writeByte(100)->writeByte(180)->writeByte(0)->writeByte(FmBandPass);
    };

    Distort distort(NULL);
    Distort::defaultSoundbank_ = distort.createSoundbank();
    Distort::defaultSoundbank_->add("bass boost.....", bassBoost->data());
    Distort::defaultSoundbank_->add("grunge.........", grunge->data());
    DEL_(bassBoost);
    DEL_(grunge);
}

void Distort::cleanUp() {
    DEL_(Distort::defaultSoundbank_);
    Distort::defaultSoundbank_ = NULL;
}