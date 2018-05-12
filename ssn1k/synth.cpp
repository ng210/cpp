//*****************************************************************************
// Simple synth: in->osc->env->flt->out
//*****************************************************************************

#include <math.h>
#include "base/memory.h"
#include "synth.h"
#include "voice.h"

NS_FW_BASE_USE
NS_SSN1K_BEGIN

//SynthCtrls::SynthCtrls() {
//	Ctrl* ctrl = (Ctrl*)this;
//	for (int i=0; i<SSN1K_CI_COUNT; i++) {
//		ctrl[i].set(.0f, .0f, 1.f);
//	}
//	//this->osc2.mix.set(SSN1K_MM_BPS);
//	//this->flt.mix.set(SSN1K_MM_BPS);
//	this->osc1.mix.set(SSN1K_MM_MUL);
//}

Synth::Synth(UINT32 voices) {
	init(voices);
}

void Synth::init(UINT32 voices) {
	smp_ = .0f;
	voiceCount_ = voices > 64 ? 64 : voices;
	//overlayValue_ = .0f;
	// create controls
	controls_ = Ctrl::createControls(SSN1K_CI_COUNT);
	//sweepCtrls = 0;
	//paSweepCtrls = new Ctrl*[256];
	// add voices
	for (UINT32 i = 0; i < voiceCount_; i++) {
		voices_[i] = NEW_(Voice, this, i);
	}
	nextVoice_ = 0;
	//bankCount_ = 0;
	//selectedBank_ = 0;
#ifdef _PROFILE

	profiler.init("Synth");
	profiler.addName(1, "ctrlSweep");
	profiler.addName(2, "lfo");
	profiler.addName(3, "env");
	profiler.addName(4, "osc");
	profiler.addName(5, "flt");
	profiler.addName(6, "overlay");

#endif
}
Synth::~Synth() {
	//delete paSweepCtrls;
	// delete voices
	for (UINT32 i = 0; i < voiceCount_; i++) {
		DEL_(voices_[i]);
	}
	destroyControls();
}

float Synth::run() {
	float smp = 0.0f;
	for (UINT32 j = 0; j<voiceCount_; j++) {
		if (voices_[j]->isActive()) {
			smp += voices_[j]->run();
		}
	}
	return smp;
}
void Synth::run(void* buffer, UINT32 start, UINT32 end) {
	for (UINT32 i = start; i<end; i++) {
		float smp = 0.0f;
		for (UINT32 j = 0; j<voiceCount_; j++) {
			if (voices_[j]->isActive()) {
				smp += voices_[j]->run();
			}
		}
		short out;
		smp *= controls_[SSN1K_CI_SynthAmp].f();
		if (smp < 1.0f) {
			if (smp > -1.0f) {
				out = (short)round(32767.0f * smp);
			} else {
				out = -32767;
			}
		} else {
			out = 32767;
		}
		short left = (short)(controls_[SSN1K_CI_SynthBal].f() * out);
		short right = (short)((1.0f - controls_[SSN1K_CI_SynthBal].f()) * out);
		((short*)buffer)[2 * i] += left;
		((short*)buffer)[2 * i + 1] += right;
	}
}
void Synth::noteOn(int note, float velocity) {
	// get free voice
	//printf("synth: %llx, note: %d, velocity: %f, duration: %d\n", this, note, velocity, duration);
	Voice* voice = voices_[nextVoice_];
	nextVoice_ = (nextVoice_ + 1) % voiceCount_;
	voice->noteOn(note, velocity);
}
void Synth::noteOff(int note) {
	bool found = false;
	for (UINT32 i = 0; i < voiceCount_; i++) {
		Voice* voice = voices_[i];
		if (voice->isActive() && voice->note().f() == note) {
			found = true;
			voice->noteOff();
		}
	}
	if (!found) {
		printf("VOICE NOT FOUND!!!\n");
	}
}
void Synth::changeProgram(int prgId) {
	UINT32* src = (UINT32*)bank_[prgId];
	UINT32* dst = (UINT32*)controls_;
	for (int i = 0; i < (sizeof(Ctrl) * SSN1K_CI_COUNT)>>2; i++) {
		dst[i] = src[i];
	}
	//for (UINT32 i = 0; i < voiceCount_; i++) {
	//	voices_[i]->setControls();
	//	//voices_[i]->reset();
	//}
}
//void Synth::setCtrlSweep(int id, float fStart, float fEnd, int iCycles) {
//	Ctrl* ctrl = paSweepCtrls[iSweepCtrls] = &ctrls[id];
//	ctrl->duration = iCycles;
//	ctrl->set(fStart, 0);
//	ctrls->delta = (fEnd - fStart)/iCycles;
//	iSweepCtrls++;
//}

//void Synth::addBank(Ctrl** bank) {
//	if (bankCount_ < 4) {
//		banks_[bankCount_++] = bank;
//	}
//}

void Synth::bank(Ctrl** bank) {
	bank_ = bank;
	changeProgram(0);
}
void Synth::setControls(Ctrl* controls, BYTE* data) {
	BYTE* ptr = data;
	int ctrlId;
	while ((ctrlId = *ptr++) != 0xFF) {
		Ctrl* ctrl = &controls[ctrlId];
		switch (ctrlId) {
		case SSN1K_CI_Tune:
		case SSN1K_CI_Env1Amp:
		case SSN1K_CI_Env1DC:
		case SSN1K_CI_Env2Amp:
		case SSN1K_CI_Env2DC:
		case SSN1K_CI_Env3Amp:
		case SSN1K_CI_Env3DC:
		case SSN1K_CI_Osc1Tune:
		case SSN1K_CI_Osc2Tune:
		case SSN1K_CI_Lfo1Amp:
		case SSN1K_CI_Lfo1Fre:
		case SSN1K_CI_Lfo2Amp:
		case SSN1K_CI_Lfo2Fre:
			// set as float
			ctrl->set(*(float*)ptr);
			ptr += sizeof(float);
			break;
		case SSN1K_CI_SynthMix:
		case SSN1K_CI_Env1Mix:
		case SSN1K_CI_Env2Mix:
		case SSN1K_CI_Env3Mix:
		case SSN1K_CI_Env4Mix:
		case SSN1K_CI_Lfo1Wav:
		case SSN1K_CI_Lfo2Wav:
		case SSN1K_CI_Osc1Wav:
		case SSN1K_CI_Osc2Wav:
		case SSN1K_CI_Osc1Mix:
		case SSN1K_CI_Osc2Mix:
		case SSN1K_CI_Lfo1Mix:
		case SSN1K_CI_Lfo2Mix:
		case SSN1K_CI_FltMix:
		case SSN1K_CI_FltMode:
			// set as byte
			ctrl->set((int)*ptr++);
			break;
		default:
			// set as float from byte / 255
			ctrl->set((float)(*ptr++ / 255.0f));
			break;
		}
	}
}


NS_SSN1K_END