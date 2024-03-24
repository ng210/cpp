#ifndef __BASS_CTRL_H
#define __BASS_CTRL_H

#include "synth/src/ui/module-ctrl.h"
#include "synth/src/device/bass-device-ext.h"

using namespace PLAYER;

	class BassCtrl : public ModuleCtrl {
		static HANDLE hBackground_;
	public:
		BassCtrl(DeviceExt* deviceExt);
		~BassCtrl();

		void create(Window* parent, char* name);

		void loadBackgroundImage();

		//LRESULT onPaint();
	};

#endif