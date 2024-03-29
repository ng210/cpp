#ifndef __DISTORT_CTRL_H
#define __DISTORT_CTRL_H

#include "synth/ui/module-ctrl.h"
#include "synth/src/module/distort.h"

namespace SYNTH_APP {
	class DistortCtrl : public ModuleCtrl {
		static HANDLE hBackground_;
	public:
		DistortCtrl(Module* module);
		virtual ~DistortCtrl();

		void create(Window* parent, char* name);
		HANDLE getBackgroundImage();
	};
}

#endif