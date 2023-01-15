#ifndef __MODULE_H
#define __MODULE_H

#include "../elem/pot.h"
#include "player/src/device.h"

NS_FW_BASE_USE
namespace SYNTH {

	class Module {
	protected: float* inputs_[8];
	protected: float* outputs_[8];
	protected: PROP_R(bool, isMono);
	protected: bool isActive_;
	protected: PotBase* pControls_;
	protected: PROP(byte*, soundBank);
	protected: PROP(int, program);
	public:
		Module();
		virtual ~Module();

		virtual void connectInput(int id, float* buffer); 
		virtual PotBase* getControl(int id);
		virtual inline float* getInput(int id);
		virtual inline float* getOutput(int id);
		virtual void initialize(byte** pData);
		virtual bool isActive();
		virtual void run(int start, int end);
		virtual void setProgram(int prgId);

		void createOutputBuffers(int count);
	};
}
#endif