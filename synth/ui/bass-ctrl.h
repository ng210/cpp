#ifndef __BASS_CTRL_H
#define __BASS_CTRL_H

#include "synth/ui/module-ctrl.h"
#include "synth/src/module/bass.h"

namespace SYNTH_APP {

	class BassCtrl : public ModuleCtrl {
		static HANDLE hBackground_;
	public:
		BassCtrl(Module* module);
		virtual ~BassCtrl();

		void create(Window* parent, char* name);

		HANDLE getBackgroundImage();

		//LRESULT onPaint();
	};
}
#endif