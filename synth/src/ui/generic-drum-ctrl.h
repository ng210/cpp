#ifndef __GENERIC_DRUM_CTRL_H
#define __GENERIC_DRUM_CTRL_H

#include "synth/src/ui/module-ctrl.h"
#include "synth/src/device/generic-drum-device-ext.h"

using namespace PLAYER;

	class GenericDrumCtrl : public ModuleCtrl {
		static HANDLE hBackground_;
	public:
		GenericDrumCtrl(DeviceExt* deviceExt);
		~GenericDrumCtrl();

		void create(Window* parent, char* name);

		void loadBackgroundImage();

		//LRESULT onPaint();
	};

#endif