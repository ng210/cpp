#ifndef __SYNTHUI_H
#define __SYNTHUI_H

#include "synth/src/ui/bass-ctrl.h"
#include "synth/src/ui/synth-ctrl.h"
#include "synth/src/ui/generic-drum-ctrl.h"
#include "synth/src/ui/mixer-ctrl.h"

	namespace SYNTH {
		class SynthUI {
		public:
			static DEVICECONTROLCREATOR synthControlCreator_;
			static void registerControlCreators();
		};
	};
#endif