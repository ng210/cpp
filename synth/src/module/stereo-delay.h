#ifndef __DELAY_H
#define __DELAY_H

#include "synth/src/module/module.h"
#include "synth/src/elem/dly.h"
#include "synth/src/elem/flt.h"

NS_FW_BASE_USE
namespace SYNTH {

	typedef struct StereoDelayValues_ {
		DlyValues left;
		Value mixLeft;
		DlyValues right;
		Value mixRight;
		FltValues flt;
	} StereoDelayValues;

	typedef struct Delayer_ {
		Dly delay;
		Flt filter;

		void initialize(DlyValues*, FltValues*);
	} Delayer;

	class StereoDelay : public Module {
	private: PROP_R(Delayer, left);
	private: PROP_R(Delayer, right);
	protected: StereoDelayValues_ values_;
		void updateFilter();
	public:
		StereoDelay();
		virtual ~StereoDelay();

		float* getOutput(int id);
		void connectInput(int id, float* buffer);
		Value* getValues();

		void run(int start, int end, BufferWriteModes mode = BufferWriteModes::Add);
	};
}

#endif