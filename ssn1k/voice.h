#ifndef __VOICE_H
#define __VOICE_H

#include "basedef.h"
#include "synth.h"

NS_SSN1K_BEGIN

class Synth;
class Voice {
private: Synth* synth_;
private: Env env1_, env2_, env3_, env4_;
private: Osc osc1_, osc2_, lfo1_, lfo2_;
private: Flt flt_;
private: PROP_R(Ctrl, note);
private: PROP_R(float, velocity);
private: PROP_R(int, id);
private: int duration_;
public:
	Voice(Synth* synth, int id);
	virtual ~Voice();
	void setControls();
	void reset();
	void noteOn(int note, float velocity);
	void noteOff();
	float run();
	bool isActive();
};

NS_SSN1K_END

#endif