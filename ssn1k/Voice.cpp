#include "Voice.h"

NS_FW_BASE_USE
NS_SSN1K_BEGIN

Voice::Voice(Synth* synth, int id) {
	synth_ = synth;
	id_ = id;
	SynthCtrls* ctrls = (SynthCtrls*)synth->controls_;
	env1_.controls_ = (Ctrl**)&ctrls->env1;
	env2_.controls_ = (Ctrl**)&ctrls->env2;
	env3_.controls_ = (Ctrl**)&ctrls->env3;
	env4_.controls_ = (Ctrl**)&ctrls->env4;
	osc1_.controls_ = (Ctrl**)&ctrls->osc1;
	osc2_.controls_ = (Ctrl**)&ctrls->osc2;
	//size_t ix = offsetof(OscCtrls, note);
	//((OscCtrls*)osc1_.controls_)->note = &note_;
	//((OscCtrls*)osc2_.controls_)->note = &note_;
	lfo1_.controls_ = (Ctrl**)&ctrls->lfo1;
	lfo2_.controls_ = (Ctrl**)&ctrls->lfo2;
	flt_.controls_ = (Ctrl**)&ctrls->flt;
	duration_ = 0;
}

Voice::~Voice() {
}

void Voice::setNote(int note, int duration, float velocity) {
	//printf("voice: %d, note: %d, velocity: %f, duration: %d\n", id_, note, velocity, duration);
	SynthCtrls* controls = (SynthCtrls*)synth_->controls_;
	note_.set((float)note, false);
	duration_ = duration;

	// key on
	velocity_ = velocity;
	env1_.setGate(velocity);
	env2_.setGate(velocity);
	env3_.setGate(velocity);
	env4_.setGate(velocity);
}
float Voice::run() {
	// run LFOs
	float lfo1 = lfo1_.run();	//1.0 - this.lfos[0].amp + (this.lfos[0].amp + this.lfos[0].run(1.0, 0.0, 0.0))/2;
	float lfo2 = lfo2_.run();
	// run main oscillators
	float am = env1_.run();
	float pm = env2_.run() + lfo2;
	float cut = env3_.run();
	float fm = lfo1 + env4_.run();
	float tune = note_.get().f + synth_->getControl(SSN1K_CI_Tune)->get().f;
	float smp1 = osc1_.run(1.0f, tune, fm, pm);
	float smp2 = osc2_.run(1.0f, tune, fm, pm, smp1);
	float out = flt_.run(am*smp2, cut);
	//float out = smp1 + smp2;
	if (duration_ > 0) {
		duration_--;
	} else {
		if (duration_ == 0) {
			//printf("voice: %d, note off\n", id_);
			// key off
			env1_.setGate(0.0f);
			env2_.setGate(0.0f);
			env3_.setGate(0.0f);
			env4_.setGate(0.0f);
			duration_ = -1;
		}
	}
	//float smp2 = osc2_.run(1.0f, lfo2, psw);
	return out;
}

bool Voice::isActive() {
	return env1_.isActive();
}

NS_SSN1K_END