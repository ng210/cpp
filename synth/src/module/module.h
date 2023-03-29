#ifndef __MODULE_H
#define __MODULE_H

#include "synth/src/elem/pot.h"
#include "player/src/device.h"
#include "synth/src/module/soundbank.h"

NS_FW_BASE_USE
namespace SYNTH {

	CREATEHANDLER1(SetSoundbank, Soundbank*);
	CREATEHANDLER1(SetProgram, int);	

	class Module {
	protected: PotBase* controls_;
	protected: PROP_R(int, controlCount);
	protected: float* inputs_[8];
	protected: float* outputs_[8];
	protected: PROP_R(bool, isMono);
	protected: bool isActive_;
	protected: PROP_R(Soundbank*, soundbank);
	protected: PROP_R(int, program);
	public:
		Module(PotBase* controls, int count);
		virtual ~Module();

		virtual void connectInput(int id, float* buffer); 
		PotBase* getControl(int id);
		void setControl(int id, S value);
		virtual inline float* getInput(int id);
		virtual inline float* getOutput(int id);
		virtual void initializeFromStream(byte** pData);
		virtual int writeToStream(Stream* stream);
		virtual int writeProgram(byte* pData);
		virtual bool isActive();
		virtual void run(int start, int end);

		SetSoundbankHandler setSoundbank;
		SetProgramHandler setProgram;

		virtual Soundbank* createSoundbank();
		void createOutputBuffers(int count);

		virtual Soundbank* getDefaultSoundbank();

		static SetSoundbankFunc soundbankSetter;
		static SetProgramFunc programSetter;
	};
}
#endif