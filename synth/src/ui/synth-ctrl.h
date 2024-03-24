#ifndef __SYNTH_CTRL_H
#define __SYNTH_CTRL_H

#include "synth/src/ui/module-ctrl.h"
#include "synth/src/device/synth-device-ext.h"

	class SynthCtrl : public ModuleCtrl {
		static HANDLE hBackground_;
	public:
		SynthCtrl(DeviceExt* deviceExt);
		~SynthCtrl();

		void create(Window* parent, char* name);

		void loadBackgroundImage();
	};

#endif