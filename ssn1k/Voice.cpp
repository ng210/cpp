#include "Voice.h"

NS_FW_BASE_USE
NS_SSN1K_BEGIN

Voice::Voice(Synth* synth, int id) {
	synth_ = synth;
	id_ = id;
	SynthCtrls* controls = (SynthCtrls*)synth->controls();
	env1_.setControls(&controls->env1);
	env2_.setControls(&controls->env2);
	env3_.setControls(&controls->env3);
	env4_.setControls(&controls->env4);
	osc1_.setControls(&controls->osc1);
	osc2_.setControls(&controls->osc2);
	size_t ix = offsetof(OscCtrls, note);
	osc1_.note(&this->note_);
	osc2_.note(&this->note_);
	lfo1_.setControls(&controls->lfo1);
	lfo2_.setControls(&controls->lfo2);
	flt_.setControls(&controls->flt);
	duration_ = 0;
}

Voice::~Voice() {
}

void Voice::setNote(int note, int duration, float velocity) {
	//printf("voice: %d, note: %d, velocity: %f, duration: %d\n", id_, note, velocity, duration);
	SynthCtrls* controls = (SynthCtrls*)synth_->controls();
	note_.set((float)note, 0);
	duration_ = duration;

	// key on
	velocity_ = velocity;
	env1_.setGate(velocity);
	env2_.setGate(velocity);
	env3_.setGate(velocity);
	//env4_.setGate(velocity);
}
float Voice::run() {
	// run LFOs
	float lfo1 = lfo1_.run(0.0f, 0.0f, 0.0f);	//1.0 - this.lfos[0].amp + (this.lfos[0].amp + this.lfos[0].run(1.0, 0.0, 0.0))/2;
	float lfo2 = lfo2_.run(0.0f, 0.0f, 0.0f);
	// run main oscillators
	float amp = env1_.run(1.0f);
	float psw = env2_.run(1.0f) + lfo2;
	float cut = env3_.run(1.0f);
	float smp1 = osc1_.run(amp, lfo1, psw);
	float smp2 = osc2_.run(amp, lfo1, psw);
	float out = flt_.run(smp1 + smp2, cut);
	if (duration_ > 0) {
		duration_--;
	} else {
		if (duration_ == 0) {
			//printf("voice: %d, note off\n", id_);
			// key off
			env1_.setGate(0.0f);
			env2_.setGate(0.0f);
			env3_.setGate(0.0f);
			//env4_.setGate(0.0f);
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