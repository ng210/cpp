#ifndef __MODULE_DEVICE_H
#define __MODULE_DEVICE_H

#include "player/src/device.h"
#include "synth/src/elem/pot.h"
#include "synth/src/module/module.h"
#include "synth/src/device/synth-adapter.h"

NS_FW_BASE_USE
using namespace PLAYER;
namespace SYNTH {


	typedef enum ModuleCommands {
		CmdSetUint8		= 2,	// ctrlId, value
		CmdSetFloat8	= 3,	// ctrlId, value
		CmdSetFloat 	= 4,	// ctrlId, value
		CmdSetNote		= 10,	// note, velocity
		CmdSetVelocity	= 11,	// velocity
		CmdSetProgram	= 12	// programId
	} ModuleCommands;

	class ModuleDevice : public Device {
	protected: PROP(int, datablockId);
	public:
		ModuleDevice(void* object, Adapter* adapter);

		void initialize(byte** pData = NULL);
		bool isActive();
		//void isActive(bool b);
		int run(int ticks);
		void setRefreshRate(float fps);
		void processCommand(byte cmd, byte*& cursor);

		PotBase* getControl(byte ctrlId);
		void setControl(byte ctrlId, S value);
		byte program();
		void setProgram(int prgId);
		Soundbank* soundbank();
		void setSoundbank(Soundbank* data);

		Module* module() { return (Module*)object_; }

#ifdef PLAYER_EDIT_MODE
		void makeCommandImpl(int command, Stream* stream, va_list args);
		int getCommandSize(byte* cmd);
		int writeToStream(Stream* stream);
#endif
		static COLLECTION_COMPARE compareToModule;
		static Map* loadSoundbanks(const char* soundbankPath, SynthAdapter* synthAdapter);

	};
}
#endif