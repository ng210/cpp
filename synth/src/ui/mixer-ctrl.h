#ifndef __MIXER_CTRL_H
#define __MIXER_CTRL_H

#include "synth/src/ui/module-ctrl.h"
#include "synth/src/device/mixer-device-ext.h"

using namespace PLAYER;

	class MixerCtrl : public ModuleCtrl {
		static HANDLE hBackground_;
	public:
		MixerCtrl(DeviceExt* deviceExt);
		~MixerCtrl();

		void create(Window* parent, char* name);

		void loadBackgroundImage();
	};


#endif