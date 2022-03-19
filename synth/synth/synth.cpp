#include "synth.h"
#include "voice.h"
#include "base/memory.h"

NS_FW_BASE_USE
using namespace SYNTH;

Synth::Synth(float smpRate, int voiceCount) {
    voices_.init(sizeof(Voice), 4);
    setVoiceCount(voiceCount);
    samplingRate(smpRate);
    controls_.amp.value_ = 1.0f;
}

Synth::~Synth() {
    var i = voices_.length();
    while (i-- > 0) {
        ((Voice*)voices_.getAt(0))->freeResources();
        voices_.removeAt(0);
    }
}

void Synth::assignControls(Pot* controls) {}

void Synth::setFromStream(byte* stream) {

}

float Synth::run(void* params) {
    return 0.0f;
}

void Synth::samplingRate(float value) {
    samplingRate_ = value;
    omega = SYNTH_THETA / (double)value;
    voices_.forEach([](void* p, UINT32 ix, Collection* array, void* samplingRate) {
        ((Voice*)p)->setSamplingRate((float*)samplingRate);
        return 1;
    }, &samplingRate_);
}

void Synth::setVoiceCount(int voiceCount) {
    var count = voiceCount;
    var oldCount = voices_.length();
    if (oldCount < count) {
        var pots = (Pot**)&controls_;
        Voice voice;
        for (var i=oldCount; i< count; i++) {
            var vp = (Voice*)voices_.add(&voice); new (vp) Voice();
            vp->envelopes[0].assignControls((Pot*)&controls_.env1);
            vp->envelopes[1].assignControls((Pot*)&controls_.env2);
            vp->envelopes[2].assignControls((Pot*)&controls_.env3);
            vp->envelopes[3].assignControls((Pot*)&controls_.env4);
            vp->lfos[0].assignControls((Pot*)&controls_.lfo1);
            vp->lfos[1].assignControls((Pot*)&controls_.lfo2);
            vp->oscillators[0].assignControls((Pot*)&controls_.osc1);
            vp->oscillators[1].assignControls((Pot*)&controls_.osc2);
            vp->filters[0].assignControls((Pot*)&controls_.flt1);
        }
    } else {
        for (var i = oldCount; i > count; --i) {
            voices_.removeAt(i);
        }
    }
}

void Synth::setNote(byte note, byte velocity) {
    if (velocity != 0) {
        // get free voice
        var candidate = (Voice*)voices_.getAt(0);
        for (int i=0; i<voices_.length(); i++) {
            var voice = (Voice*)voices_.getAt(i);
            if (candidate->getTicks() < voice->getTicks()) {
                candidate = voice;
            }
            if (!voice->isActive()) {
                candidate = voice;
                break;
            }
        }
        candidate->setNote(note, velocity);
    } else {
        for (int i=0; i<voices_.length(); i++) {
            var voice = (Voice*)voices_.getAt(i);
            if (voice->envelopes[0].phase() < EnvPhase::Down && voice->note().value_.b == note) {
                voice->setNote(note, 0);
                break;
            }
        }
    }
}

PotBase* Synth::getControl(int controlId) {
    var p = (PotBase*)&controls_;
    return p + controlId;
}

void Synth::setProgram(int prgId) {
    var sb = soundBank_;
    isActive = true;
    var count = (int)*sb;
    if (prgId < count) {
        var offset = *(short*)(sb + 1 + 16 * prgId + 14);
        sb += offset;
        count = *sb++;
        for (var i = 0; i < count; i++) {
            var iid = *sb++;
            var pot = getControl(iid);
            pot->setFromStream(sb);
        }
    }
}

void Synth::run(short* buffer, int start, int end) {
    if (isActive) {
        for (var i = start; i < end; i++) {
            var smp = 0.0f;
            for (int j = 0; j < voices_.length(); j++) {
                var voice = (Voice*)voices_.getAt(j);
                if (voice->isActive()) {
                    smp += voice->run();
                }
            }
            smp *= controls_.amp.value_.f;
            // clip
            if (smp < -1.0f) smp = -1.0f;
            else if (smp > 1.0f) smp = 1.0f;
            buffer[i] += (short)(smp * 32767.0f);
        }
    }
}

/*

Synth.prototype.setControl = function(controlId, value) {
    this.getControl(controlId).set(value);
    //console.log('setcontrol:', controlId, value);
}
Synth.prototype.run = function(left, right, start, end) {
    if (this.isActive) {
        for (var i=start; i<end; i++) {
            var smp = 0;
            for (var j=0; j<this.voices.length; j++) {
                smp += this.controls.amp.value*this.voices[j].run(1);
            }
            left[i] += smp;
            right[i] += smp;
        }
        //console.log(buffer[start]);
    }
}
Synth.prototype.setProgram = function setProgram(id) {
    var sb = this.soundBank;
    this.isActive = true;
    var count = sb.readUint8(0);
    if (id < count) {
        var offset = 1 + 16*id;
        // this.selectedProgram = sb.readString(offset);
        offset = sb.readUint16(offset + 14);
        count = sb.readUint8(offset);
        for (var i=0; i<count; i++) {
            var iid = sb.readUint8();
            var ctrl = this.getControl(iid);
            if (ctrl) ctrl.setFromStream(sb);
            else debugger
        }
    }
}

Synth.prototype.createControls = function createControls() {
    this.controls = {
        amp: new psynth.PotF8(0, 1, .8),
        env1: psynth.Env.createControls(),
        env2: psynth.Env.createControls(),
        env3: psynth.Env.createControls(),
        env4: psynth.Env.createControls(),
        lfo1: psynth.LFO.createControls(),
        lfo2: psynth.LFO.createControls(),
        osc1: psynth.Osc.createControls(),
        osc2: psynth.Osc.createControls(),
        flt1: psynth.Filter.createControls(),
    };

    this.controls.lfo1.amp.init(0, 1, 0.01);
    this.controls.lfo1.fre.init(0, 1000, 0.1);
    this.controls.lfo2.amp.init(0, 100, 0.1);
    this.controls.lfo2.fre.init(0, 1000, 0.1);
    this.controls.env3.amp.set(1.0);
    this.controls.env4.amp.set(0.0);
    this.controls.env4.amp.max = 5000;
    this.controls.env4.dc.max = 5000;
    this.controls.flt1.mode.init(1, 7, 1);
    this.controls.flt1.cut.set(0.0);

    this.idToControl = [];
    var keys1 = Object.keys(this.controls);
    for (var i=0; i<keys1.length; i++) {
        var key1 = keys1[i];
        var obj = this.controls[key1];
        if (inherits(obj, psynth.PotBase)) {
            this.idToControl.push(obj);
        } else {
            var keys2 = Object.keys(obj);
            for (var j=0; j<keys2.length; j++) {
                var key2 = keys2[j];
                if (inherits(obj[key2], psynth.PotBase)) {
                    this.idToControl.push(obj[key2]);
                }
            }
        }
    }
};

Synth.controls = (function() {
    var map = {};
    var synth = new Synth(sound.SamplingRate, 1);
    var id = 0;
    var keys1 = Object.keys(synth.controls);
    for (var i=0; i<keys1.length; i++) {
        var key1 = keys1[i];
        var obj = synth.controls[key1];
        if (inherits(obj, psynth.PotBase)) {
            map[key1] = id++;
        } else {
            var keys2 = Object.keys(obj);
            for (var j=0; j<keys2.length; j++) {
                var key2 = keys2[j];
                if (inherits(obj[key2], psynth.PotBase)) {
                    map[key1+key2] = id++;
                }
            }
        }
    }
    return map;
})();

// Synth.prototype.setup = function(values) {
//     for (var i=0; i<values.length; i+=2) {
//         var ctrlId = values[i];
//         var key = Object.keys(psynth.Synth.controls).find(
//             function(k) {
//                 return psynth.Synth.controls[k] == ctrlId;
//             }
//         );
//         if (key === undefined) {
//             console.log('No control with the id ' + ctrlId + ' was found!');
//         }
//         this.getControl(values[i]).value = values[i+1];
//     }
// };
*/