#ifndef __ENV_H
#define __ENV_H

#include "synth/src/elem/elem.h"

NS_FW_BASE_USE
namespace SYNTH {

	#define SMOOTH(x) x*(3.0*x - 2.0*x*x)

	typedef enum EnvPhase {
		Up,
		Del,
		Atk,
		Dec,
		Hld,
		Sus,
		Down,
		Rel,
		Idle,
		Kill
	} EnvPhase;

	class Env : public Elem {
	protected: float velocity_;
	protected: PROP_R(int, ticks);
	protected: byte gate_;
	protected: double overlaySmp_;
	protected: double timer_;
	protected: Value* values_;
	protected:
		double rate_;
		//double ai_[2], fc_;
		//void applyLPF();
	public: PROP_R(EnvPhase, phase);
	public: PROP_R(double, smp);
	public:
		Env();

		virtual void setGate(byte velocity) = 0;
		inline bool isActive() { return phase_ != EnvPhase::Idle; }
		void kill();

		void setValues(void*);

		static double attackRates[256];
		static double releaseRates[256];
		static double releaseRatesExp[256];
		static double fadoutFactor;
		static void initialize();
	};

}

#endif
