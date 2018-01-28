//*****************************************************************************
// Simple synth: in->osc->env->flt->out
//*****************************************************************************

#include "base/MemoryMgr.h"
#include "Synth.h"
#include "Voice.h"
#include <math.h>

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

Synth::Synth(size_t voices) {
	init(voices);
}

void Synth::init(size_t voices) {
	smp_ = .0f;
	voiceCount_ = voices > 64 ? 64 : voices;
	//overlayValue_ = .0f;
	// create controls
	controls_ = MALLOC(Ctrl*, SSN1K_CI_COUNT);
	Ctrl* controls = MALLOC(Ctrl, SSN1K_CI_COUNT);
	for (int i = 0; i < SSN1K_CI_COUNT; i++) {
		controls_[i] = &controls[i];
		controls[i].set(0.0, 0.0, 1.0f);
	}
	//sweepCtrls = 0;
	//paSweepCtrls = new Ctrl*[256];
	// add voices
	for (int i = 0; i < voiceCount_; i++) {
		voices_[i] = NEW_(Voice, this, i);
	}
	nextVoice_ = 0;
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
	for (size_t i = 0; i < voiceCount_; i++) {
		DEL_(voices_[i]);
	}
	FREE(controls_[0]);
	FREE(controls_);
}

//int Synth::isActive() {
//	int ret = this->env1.isActive();
//	return ret;
//}

//void Synth::setCtrls(SynthCtrls* controls) {
//	Mdl::setCtrls((Ctrl*)pCtrls);
//	env1_.setCtrls((Ctrl*)&((SynthCtrls*)pCtrls)->env1);
//	env2_.setCtrls((Ctrl*)&((SynthCtrls*)pCtrls)->env2);
//	env3_.setCtrls((Ctrl*)&((SynthCtrls*)pCtrls)->env3);
//	env4_.setCtrls((Ctrl*)&((SynthCtrls*)pCtrls)->env4);
//	osc1_.setCtrls((Ctrl*)&((SynthCtrls*)pCtrls)->osc1);
//	osc2_.setCtrls((Ctrl*)&((SynthCtrls*)pCtrls)->osc2);
//	lfo1_.setCtrls((Ctrl*)&((SynthCtrls*)pCtrls)->lfo1);
//	lfo2_.setCtrls((Ctrl*)&((SynthCtrls*)pCtrls)->lfo2);
//	//flt_.setCtrls((Ctrl*)&((SynthCtrls*)pCtrls)->flt);
//}

void Synth::setup(CtrlSetting* data) {
	while (data->id != -1) {
		Ctrl* ctrl = getControl(data->id);
		ctrl->set(data->value);
		data++;
	}
}

Ctrl* Synth::getControl(size_t id) {
	return controls_[id];
}

void Synth::run(void* buffer, size_t start, size_t end) {
	for (size_t i = start; i<end; i++) {
		float smp = 0.0f;
		for (size_t j = 0; j<voiceCount_; j++) {
			if (voices_[j]->isActive()) {
				smp += voices_[j]->run();
			}
		}
		short out = (short)floor(32767.0f * smp * controls_[SSN1K_CI_SynthAmp]->get().f);
		short left = (short)(controls_[SSN1K_CI_SynthBal]->get().f * out);
		short right = (short)((1.0f - controls_[SSN1K_CI_SynthBal]->get().f) * out);
		((short*)buffer)[2 * i] += left;
		((short*)buffer)[2 * i + 1] += right;
	}
}
void Synth::run(void* buffer, size_t ix) {
	float smp = 0.0f;
	for (size_t j = 0; j<voiceCount_; j++) {
		if (voices_[j]->isActive()) {
			smp += voices_[j]->run();
		}
	}
	short out = (short)floor(32767.0f * smp * controls_[SSN1K_CI_SynthAmp]->get().f);
	short left = (short)(controls_[SSN1K_CI_SynthBal]->get().f * smp);
	short right = (short)((1.0f - controls_[SSN1K_CI_SynthBal]->get().f) * smp);
	((short*)buffer)[2 * ix] += left;
	((short*)buffer)[2 * ix + 1] += right;
}

//float Synth::run(float in) {
//#ifdef _PROFILE
//	profiler.start();
//	profiler.enter(1);
//#endif
//	// update sweepCtrls
//	//for (int i=0; i<iSweepCtrls; i++) {
//	//	paSweepCtrls[i]->change();
//	//}
//#ifdef _PROFILE
//	profiler.leave(1);
//#endif
//	float prevSmp = smp_;
//
//	//if (env1.isActive())
//	{
//		SynthCtrls* ctrls = (SynthCtrls*)this->ctrls_;
//#ifdef _PROFILE
//	profiler.enter(2);
//#endif
//		float lfo1 = lfo1.run(0.0f, 0, 0);
//		float lfo2 = lfo2.run(0.0f, 0, 0);
//
//		ctrls->env1.dc.set(fLfo1, 0);
//		ctrls->env2.dc.set(fLfo2, 0);
//#ifdef _PROFILE
//	profiler.leave(2);
//#endif
//
//#ifdef _PROFILE
//	profiler.enter(3);
//#endif
//		float fAM = env1.run(ctrls->amp.get().fValue);
//		float fFM = 5000 * env2.run(0);
//		float fPM = env3.run(0);
//		float fCM = 2*env4.run(0);
//#ifdef _PROFILE
//	profiler.leave(3);
//#endif
//#ifdef _PROFILE
//	profiler.enter(4);
//#endif
//		smp = osc1.run(fAM, fFM, fPM);
//		smp = osc2.run(smp, fFM, fPM);
//#ifdef _PROFILE
//	profiler.leave(4);
//#endif
//#ifdef _PROFILE
//	profiler.enter(5);
//#endif
//		smp = flt.run(smp, fCM);
//#ifdef _PROFILE
//	profiler.leave(5);
//#endif
//		//smp = env4.smp;
//#ifdef _PROFILE
//	profiler.enter(6);
//#endif
//		// Adding overlay
//		if (env1.iOverlayCounter > 0)
//		{
//			if (env1.iOverlayCounter == OVERLAY_LENGTH-1)
//			{
//				fOverlayValue = fPrevSmp;
//			}
//			float fFact = smooth((float)env1.iOverlayCounter/OVERLAY_LENGTH);
//			smp = fOverlayValue*fFact + (1.0f - fFact) * smp;
//		}
//	}
//#ifdef _PROFILE
//	profiler.leave(6);
//	profiler.end();
//#endif
//	return smp;
//}

void Synth::setNote(int note, int duration, float velocity) {
	// get free voice
	Voice* voice = voices_[nextVoice_];
	nextVoice_ = (nextVoice_ + 1) % voiceCount_;
	voice->setNote(note, duration, velocity);
}

//void Synth::setCtrlSweep(int id, float fStart, float fEnd, int iCycles) {
//	Ctrl* ctrl = paSweepCtrls[iSweepCtrls] = &ctrls[id];
//	ctrl->duration = iCycles;
//	ctrl->set(fStart, 0);
//	ctrls->delta = (fEnd - fStart)/iCycles;
//	iSweepCtrls++;
//}

NS_SSN1K_END