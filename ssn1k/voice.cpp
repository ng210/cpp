#include "voice.h"

NS_SSN1K_BEGIN

Voice::Voice(Synth* synth, int id) {
	synth_ = synth;
	id_ = id;
	duration_ = 0;
	setControls();
}
void Voice::setControls() {
	SynthCtrls* ctrls = (SynthCtrls*)synth_->controls_;
	env1_.controls_ = (Ctrl*)&ctrls->env1;
	env2_.controls_ = (Ctrl*)&ctrls->env2;
	env3_.controls_ = (Ctrl*)&ctrls->env3;
	env4_.controls_ = (Ctrl*)&ctrls->env4;
	osc1_.controls_ = (Ctrl*)&ctrls->osc1;
	osc2_.controls_ = (Ctrl*)&ctrls->osc2;
	//size_t ix = offsetof(OscCtrls, note);
	//((OscCtrls*)osc1_.controls_)->note = &note_;
	//((OscCtrls*)osc2_.controls_)->note = &note_;
	lfo1_.controls_ = (Ctrl*)&ctrls->lfo1;
	lfo2_.controls_ = (Ctrl*)&ctrls->lfo2;
	flt_.controls_ = (Ctrl*)&ctrls->flt;
}
void Voice::reset() {
	env1_.reset();
	env2_.reset();
	env3_.reset();
	env4_.reset();
}
Voice::~Voice() {
}

void Voice::noteOn(int note, float velocity) {
	//printf("voice: %d, note: %d, velocity: %f, duration: %d\n", id_, note, velocity, duration);
	SynthCtrls* controls = (SynthCtrls*)synth_->controls_;
	noteOff();

	note_.set((float)note);
	// key on
	velocity_ = velocity;
	env1_.setGate(velocity);
	env2_.setGate(velocity);
	env3_.setGate(velocity);
	env4_.setGate(velocity);
}
void Voice::noteOff() {
	SynthCtrls* controls = (SynthCtrls*)synth_->controls_;
	// force key off
	env1_.setGate(0.0f);
	env2_.setGate(0.0f);
	env3_.setGate(0.0f);
	env4_.setGate(0.0f);
}
float Voice::run() {
	// run LFOs
	float lfo1 = lfo1_.run();	//1.0 - this.lfos[0].amp + (this.lfos[0].amp + this.lfos[0].run(1.0, 0.0, 0.0))/2;
	float lfo2 = lfo2_.run();
	// run main oscillators
	float am = env1_.run();
	float pm = env2_.run() + lfo2;
	float cut = env3_.run();
	float fm = /*lfo1 +*/ env4_.run();
	float tune = note_.f() + lfo1 + synth_->getControl(SSN1K_CI_Tune)->f();
	float smp1 = osc1_.run(1.0f, tune, fm, pm);
	float smp2 = osc2_.run(1.0f, tune, fm, pm, smp1);
	float out = flt_.run(am*smp2, cut);
	//if (duration_ > 0) {
	//	duration_--;
	//} else {
	//	if (duration_ == 0) {
	//		//printf("voice: %d, note off\n", id_);
	//		// key off
	//		env1_.setGate(0.0f);
	//		env2_.setGate(0.0f);
	//		env3_.setGate(0.0f);
	//		env4_.setGate(0.0f);
	//		duration_ = -1;
	//	}
	//}

	//float smp2 = osc2_.run(1.0f, lfo2, psw);
	return out;
}
bool Voice::isActive() {
	return env1_.isActive();
}

NS_SSN1K_END