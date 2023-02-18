#ifndef __BASS_CTRL_H
#define __BASS_CTRL_H

#include "synth/ui/module-ctrl.h"
#include "synth/src/module/bass.h"

namespace SYNTH_UI {

	class BassCtrl : public ModuleCtrl {
	public:
		BassCtrl(Module* module);
		~BassCtrl();

		void create(Window* parent, char* name);

		//LRESULT onPaint();
	};
}
#endif