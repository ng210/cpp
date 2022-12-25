#ifndef __DELAY_H
#define __DELAY_H

#include "module.h"
#include "../elem/dly.h"

NS_FW_BASE_USE
namespace SYNTH {

	typedef struct DelayCtrls_ {
		PotF8 feedbackLeft;
		PotF delayLeft;
		PotF8 feedbackRight;
		PotF delayRight;
		PotF8 mixLeft;
		PotF8 mixRight;
	} DelayCtrls;	

	class StereoDelay : public Module {
	protected: PROP_R(DelayCtrls, controls);
	private: Dly left_;
	private: Dly right_;

	public:
		StereoDelay(float* samplingRate);
		virtual ~StereoDelay();

		void initialize(byte** pData);
		void connectInput(int id, float* buffer);
		void run(int start, int end);

		void setDelayLeft(float delay);
		void setDelayRight(float delay);
	};
}

#endif