#ifndef __DISTORT_H
#define __DISTORT_H

#include "collection/array.h"
#include "synth/src/module/module.h"
#include "synth/src/elem/clp.h"
#include "synth/src/elem/flt.h"

NS_FW_BASE_USE
namespace SYNTH {

	typedef struct DistortValues_ {
		ClpValues clp;
		FltValues flt;
	} DistortValues;



	class Distort : public Module {
	private: Clp clp_;
	private: Flt flt_;
	protected: DistortValues values_;
	public:
		Distort();
		virtual ~Distort();

		float* getOutput(int id);
		Value* getValues();

		void run(int start, int end, BufferWriteModes mode = BufferWriteModes::Add);
	};
}

#endif