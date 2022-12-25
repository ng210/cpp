#ifndef __DISTORT_H
#define __DISTORT_H

#include "module.h"
#include "../elem/clp.h"
#include "../elem/flt.h"

NS_FW_BASE_USE
namespace SYNTH {

	typedef struct DistortCtrls_ {
		PotF amp;
		PotF8 lvl;
		Pot cut;	// tone
		PotF8 res;	// contour
		PotF8 mod;
		Pot mode;	// mode
	} DistortCtrls;

#define DistortCtrlsCount (sizeof(DistortCtrls) / sizeof(Pot))


	class Distort : public Module {
	private: PROP_R(DistortCtrls, controls);
	private: Clp clp_;
	private: Flt flt_;
	public:
		Distort(float* samplingRate);
		virtual ~Distort();
		void initialize(byte** pData);
		inline float* getOutput(int id);
		void run(int start, int end);
		void updateFilter();
	};
}

#endif