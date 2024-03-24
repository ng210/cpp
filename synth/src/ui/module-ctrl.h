#ifndef __MODULE_CTRL_H
#define __MODULE_CTRL_H

#include "player/src/device-ctrl.h"
#include "synth/src/device/module-device.h"
#include "base/stream.h"
#include "player/src/sequence.h"

using namespace SYNTH;
using namespace PLAYER;

	class ModuleCtrl : public DeviceCtrl {
		static DEVICECONTROLCREATOR moduleControlCreator_;

		static char* windowClassName_;
		static ATOM windowClass_;

		static int presetBankSetter(void*, PresetBank*, void* = NULL);
		static int presetSetter(void*, int, void* = NULL);
		//static CBSELECTITEMPROC onSelectProgram;
	protected:
		//HANDLE hBackground_;

	public:
		ModuleCtrl();
		ModuleCtrl(DeviceExt* deviceExt);
		~ModuleCtrl();

		char* const registerWindowClass();
		//void create(Window* parent, char* name, LONG style = 0, DWORD exStyle = 0);
	};

#endif
