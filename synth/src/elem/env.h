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
	public: double timer_;
	protected:
		double rate_;
		double ai_[2], fc_;
		void applyLPF();
	public: PROP_R(EnvPhase, phase);
	public: PROP_R(double, smp);
	public:
		Env(int count);

		void assignControls(PotBase* controls);
		void setFromStream(byte*& stream);
		virtual void setGate(byte velocity) = 0;
		inline bool isActive() { return phase_ != EnvPhase::Idle; }

		static double attackRates[256];
		static double releaseRates[256];
		static double releaseRatesExp[256];
		static void initialize();
	};

}

#endif
