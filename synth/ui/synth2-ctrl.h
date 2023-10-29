#ifndef __SYNTH2_CTRL_H
#define __SYNTH2_CTRL_H

#include "synth/ui/module-ctrl.h"
#include "synth/src/module/synth2.h"

namespace SYNTH_APP {

	class Synth2Ctrl : public ModuleCtrl {
		static HANDLE hBackground_;
	public:
		Synth2Ctrl(Module* module);
		virtual ~Synth2Ctrl();

		void create(Window* parent, char* name);

		HANDLE getBackgroundImage();
	};
}
#endif