#include "soundlib/src/soundplayer.h"
#include "base/memory.h"
#include "player/src/player.h"
#include "synth/src/device/synth-device.h"
#include "synth/src/module/synth.h"

using namespace SYNTH;

SynthDevice::SynthDevice(SynthAdapter* adapter, Player* player) : SynthBaseDevice(adapter, player, NEW_(Synth, 3)) {
	type(SynthDevices::DeviceSynth);
    inputs_ = (InputBase*)&synthInputs;
    inputCount_ = sizeof(SynthInputs) / sizeof(InputBase);
    setupAdsr(&synthInputs.amAdsr);
    setupAdsr(&synthInputs.fmAdsr);
    setupAdsr(&synthInputs.pmAdsr);
    setupAdsr(&synthInputs.ftAdsr);
    setupLfo(&synthInputs.lfo1);
    setupLfo(&synthInputs.lfo2);
    setupOsc(&synthInputs.osc1);
    setupOsc(&synthInputs.osc2);
    setupFlt(&synthInputs.flt1);
    assignInputs();
}

SynthDevice::~SynthDevice() {
	if (object_ != NULL) {
		DEL_((Synth*)object_);
		object_ = NULL;
	}
}

int SynthDevice::getPresetBankSize() {
    return SynthInputSize;
}

SynthBase* SynthDevice::synth() {
	return (SynthBase*)object_;
}

byte _defaultSynthPresetBank[] = {
    2,											// 2 preset in the bank

    DB('S'), DB('y'), DB('n'), DB('t'), DB('h'), DB('0'), DB('1'), DB('.'),
    DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB(0),	// 1st name
    DB('P'), DB('i'), DB('a'), DB('n'), DB('o'), DB('0'), DB('1'), DB('.'),
    DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB('.'), DB(0),	// 2nd name

    // Synth
    DF(0.9f), DB(4), DB(12), DB(210), DB(60),                   //amAdsrAmp, amAdsrAtk, amAdsrDec, amAdsrSus, amAdsrRel
    DF(400.0f), DB(0), DB(6), DB(0), DB(0),                     //fmAdsrAmp, fmAdsrAtk, fmAdsrDec, fmAdsrSus, fmAdsrRel
    DF(0.2f), DB(0), DB(14), DB(128), DB(44),                   //pmAdsrAmp, pmAdsrAtk, pmAdsrDec, pmAdsrSus, pmAdsrRel
    DF(0.1f), DB(8), DB(20), DB(20), DB(20),                    //ftAdsrAmp, ftAdsrAtk, ftAdsrDec, ftAdsrSus, ftAdsrRel
    DF(0.2f), DF(3.5f),                                         //amLfoAmp, amLfoFre
    DF(0.015f), DF(6.0f),                                       //fmLfoAmp, fmLfoFre
    DB(130), DF(0.0f), DB(0), DB(0), DB(120), DB(WfSaw),        //osc1Amp, osc1Fre, osc1Note, osc1Tune, osc1Psw, osc1Wave
    DB(100), DF(0.0f), DB(0), DB(12), DB(90), DB(WfSaw),        //osc2Amp, osc2Fre, osc2Note, osc2Tune, osc2Psw, osc2Wave
    DB(10), DB(60), DB(FmLowPass|FmBandPass),                   //flt1Cut, flt1Res, flt1Mode

    // Piano
    DF(0.9f), DB(1), DB(32), DB(120), DB(60),                   //amAdsrAmp, amAdsrAtk, amAdsrDec, amAdsrSus, amAdsrRel
    DF(2.0f), DB(0), DB(2), DB(1), DB(4),                       //fmAdsrAmp, fmAdsrAtk, fmAdsrDec, fmAdsrSus, fmAdsrRel
    DF(0.2f), DB(0), DB(24), DB(128), DB(120),                  //pmAdsrAmp, pmAdsrAtk, pmAdsrDec, pmAdsrSus, pmAdsrRel
    DF(0.4f), DB(2), DB(32), DB(100), DB(42),                   //ftAdsrAmp, ftAdsrAtk, ftAdsrDec, ftAdsrSus, ftAdsrRel
    DF(0.06f), DF(4.4f),                                        //amLfoAmp, amLfoFre
    DF(0.03f), DF(0.4f),                                        //fmLfoAmp, fmLfoFre
    DB(120), DF(0.0f), DB(0), DB(0), DB(168), DB(WfTriangle),   //osc1Amp, osc1Fre, osc1Note, osc1Tune, osc1Psw, osc1Wave
    DB(100), DF(2.2f), DB(0), DB(12), DB(180), DB(WfTriangle),  //osc2Amp, osc2Fre, osc2Note, osc2Tune, osc2Psw, osc2Wave
    DB(80), DB(40), DB(FmLowPass| FmBandPass)                   //flt1Cut, flt1Res, flt1Mode
};

PresetBank* SynthDevice::getDefaultPresetBank() {
    return NEW_(PresetBank, getPresetBankSize(), _defaultSynthPresetBank);
}