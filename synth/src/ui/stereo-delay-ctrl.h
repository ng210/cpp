#ifndef __STEREODELAY_CTRL_H
#define __STEREODELAY_CTRL_H

#include "synth/ui/module-ctrl.h"
#include "synth/src/module/stereo-delay.h"

namespace SYNTH_APP {
	class StereoDelayCtrl : public ModuleCtrl {
		static HANDLE hBackground_;
	public:
		StereoDelayCtrl(Module* module);
		virtual ~StereoDelayCtrl();

		void create(Window* parent, char* name);
		HANDLE getBackgroundImage();
	};
}

#endif