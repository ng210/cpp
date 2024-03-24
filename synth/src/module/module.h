#ifndef __MODULE_H
#define __MODULE_H

#include "basedef.h"
#include "player/src/value.h"

using namespace PLAYER;
namespace SYNTH {

	class Module {
	protected: float* inputs_[8];
	protected: float* outputs_[8];
	protected: PROP_R(bool, isMono);
	protected: bool isActive_;
	public:
		Module();
		virtual ~Module();
		
		virtual Value* getValues() = 0;

		// routing
		virtual void connectInput(int id, float* buffer);
		virtual inline float* getInput(int id);
		virtual inline float* getOutput(int id);
		void createOutputBuffers(int count);
		
		virtual bool isActive();
		virtual void run(int start, int end);
	};
}
#endif