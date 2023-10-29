#ifndef __DELAY_H
#define __DELAY_H

#include "synth/src/module/module.h"
#include "synth/src/elem/dly.h"
#include "synth/src/elem/flt.h"

NS_FW_BASE_USE
namespace SYNTH {

	typedef struct StereoDelayCtrls_ {
		PotF8 feedbackLeft;
		PotF delayLeft;
		PotF8 feedbackRight;
		PotF delayRight;
		PotF8 mixLeft;
		PotF8 mixRight;
		Pot cut;
		PotF8 res;
		Pot mode;
	} StereoDelayCtrls;

	typedef enum StereoDelayCtrlId {
		stdlFeedbackLeft,
		stdlDelayLeft,
		stdlFeedbackRight,
		stdlDelayRight,
		stdlMixLeft,
		stdlMixRight,
		stdlCut,
		stdlRes,
		stdlMode
	} StereoDelayCtrlId;

	#define StereoDelayCtrlCount sizeof(StereoDelayCtrls_) / sizeof(PotBase)

	class StereoDelay : public Module {
		static Soundbank* defaultSoundbank_;
		//static SETCONTROLPROC setControlProc;
	private: Dly left_, right_;
	private: Flt fltL_, fltR_;

	public:
		StereoDelay();
		virtual ~StereoDelay();

		StereoDelayCtrls controls;

		//void initializeFromStream(byte** pData);
		void connectInput(int id, float* buffer);
		void run(int start, int end);

		void setDelayLeft();
		void setDelayRight();

		Soundbank* getDefaultSoundbank();

		void updateFilter();

		static void prepare();
		static void cleanUp();

		static int delayLeftSetter(void*, S, void* = NULL);
		static int delayRightSetter(void*, S, void* = NULL);
	};
}

#endif