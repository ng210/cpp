#ifndef __MODULE_H
#define __MODULE_H

#include "basedef.h"
#include "player/src/value.h"

using namespace PLAYER;
namespace SYNTH {

	typedef enum BufferWriteModes {
		None = 0,
		Overwrite = 1,
		Add = 2
	} BufferWriteModes;

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
		virtual float* getInput(int id);
		virtual float* getOutput(int id);
		virtual void setOutput(int id, float* buffer);
		float* createOutputBuffer(bool isStereo = false);
		void createOutputBuffers(int count);
		
		virtual bool isActive();
		virtual void run(int start, int end, BufferWriteModes mode = BufferWriteModes::Overwrite);
	};
}
#endif