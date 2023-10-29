#ifndef __DISTORT_H
#define __DISTORT_H

#include "collection/array.h"
#include "synth/src/module/module.h"
#include "synth/src/elem/clp.h"
#include "synth/src/elem/flt.h"

NS_FW_BASE_USE
namespace SYNTH {

	typedef struct DistortCtrls_ {
		PotF amp;
		PotF8 lvl;
		Pot cut;	// tone
		PotF8 res;	// contour
		Pot mode;	// mode
	} DistortCtrls;

	typedef enum DistortCtrlId {
		distAmp,
		distLvl,
		distCut,
		distRes,
		distMode
	} DistortCtrlId;

#define DistortCtrlsCount (sizeof(DistortCtrls) / sizeof(Pot))


	class Distort : public Module {
		static Soundbank* defaultSoundbank_;
	
	private: Clp clp_;
	private: Flt flt_;
	public:
		DistortCtrls controls;

		Distort();
		virtual ~Distort();
		//void initializeFromStream(byte** pData);
		inline float* getOutput(int id);
		void run(int start, int end);
		void updateFilter();

		Soundbank* getDefaultSoundbank();

		static void prepare();
		static void cleanUp();
	};
}

#endif