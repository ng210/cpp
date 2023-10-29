#ifndef __UI_MGR_H
#define __UI_MGR_H

#include "component.h"
#include "collection/parray.h"
//#include "synth/ui/synth-app.h"

namespace SYNTH_APP {

	class SynthApp;

	class UiManager : public Component<SynthApp> {
		PROP_R(PArray, controls);
	public:
		UiManager(SynthApp* app);
		virtual ~UiManager();
		//int updateLayout();
	};
}

#endif