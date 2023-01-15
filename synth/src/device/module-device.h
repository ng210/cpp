#ifndef __MODULE_DEVICE_H
#define __MODULE_DEVICE_H

#include "player/src/device.h"

NS_FW_BASE_USE
using namespace PLAYER;
namespace SYNTH {


	typedef enum ModuleCommands {
		CmdSetUint8		= 2,
		CmdSetFloat8	= 3,
		CmdSetFloat 	= 4,
	} ModuleCommands;

	class ModuleDevice : public Device {
	public:
		ModuleDevice(void* object, Adapter* adapter);

		void initialize(byte** pData = NULL);
		bool isActive();
		//void isActive(bool b);
		int run(int ticks);
		void setRefreshRate(float fps);
		void processCommand(byte cmd, byte*& cursor);

		void setControl(byte ctrlId, byte value);
		void setControl(byte ctrlId, float value);

#ifdef PLAYER_EDIT_MODE
		void makeCommandImpl(int command, byte*& stream, va_list args);
		int getCommandSize(byte cmd, byte* args);
#endif
		static int compareToModule(void* moduleDevice, UINT32 ix, Collection* collection, void* module);

	};
}
#endif