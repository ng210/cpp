#ifndef __MIXER_CTRL_H
#define __MIXER_CTRL_H

#include "synth/ui/module-ctrl.h"
#include "synth/src/module/mixer.h"

namespace SYNTH_APP {
	class MixerCtrl : public ModuleCtrl {
		Stream* createVerticalLayout();
		Stream* createHorizontalLayout();
		void updateLayout();
	public:
		MixerCtrl(Module* module);
		virtual ~MixerCtrl();

		void create(Window* parent, char* name);
	};
}

#endif