#ifndef __DLY_H
#define __DLY_H

#include "elem.h"

NS_FW_BASE_USE
namespace SYNTH {

#define DELAY_MAX 5000	// milliseconds

	typedef struct DlyCtrls {
		PotF8 feedback;
		PotF delay;
	} DlyCtrls;


	class Dly : public Elem {
	protected: PROP_R(DlyCtrls*, controls);
	private: float* buffer_;
	private: float length_;
	private: float cursor_;
	//private: float* input_;
	protected: PROP_R(float*, samplingRate);
	public:
		Dly();
		virtual ~Dly();

		void assignControls(PotBase* controls);
		void setFromStream(byte* stream);
		//void connect(int id, void* input);
		float run(Arg params = (void*)NULL);
		//void run(float* buffer, int start, int end);

		void setDelay(float delay);
		void samplingRate(float* smpRate);
	};

}
#endif