#ifndef __SYNTH_CTRL_H
#define __SYNTH_CTRL_H

#include "synth/ui/module-ctrl.h"
#include "synth/src/module/synth.h"

namespace SYNTH_UI {

	class SynthCtrl : public ModuleCtrl {
		static HANDLE hBackground_;
	public:
		SynthCtrl(Module* module);
		~SynthCtrl();

		void create(Window* parent, char* name);

		HANDLE getBackgroundImage();
	};
}
#endif