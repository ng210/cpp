#ifndef __ENV_H
#define __ENV_H

#include "synth/src/elem/elem.h"

NS_FW_BASE_USE
namespace SYNTH {

	#define SMOOTH(x) x*(3.0*x - 2.0*x*x)

	typedef enum class EnvPhase {
		Up,
		Del,
		Atk,
		Dec,
		Hld,
		Sus,
		Down,
		Rel,
		Idle
	} EnvPhase;

	class Env : public Elem {
	protected: float velocity_;
	protected: PROP_R(int, ticks);
	protected: double timer_;
	protected: double rate_;
	protected: PROP_R(EnvPhase, phase);
	public:
		Env();

		virtual void setGate(byte velocity) = 0;
		inline bool isActive() { return phase_ != EnvPhase::Idle; }

		static double attackRates[256];
		static double releaseRates[256];
		static double releaseRatesExp[256];
		static void initialize();
	};

}

#endif
