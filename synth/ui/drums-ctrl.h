#ifndef __DRUMS_CTRL_H
#define __DRUMS_CTRL_H

#include "synth/ui/module-ctrl.h"
#include "synth/src/module/drums.h"

namespace SYNTH_UI {

	class GenericDrumCtrl : public ModuleCtrl {
		static HANDLE hBackground_;
	public:
		GenericDrumCtrl(Module* module);
		~GenericDrumCtrl();

		void create(Window* parent, char* name);
		//LRESULT onPaint();
		HANDLE getBackgroundImage();

		//static SETPROGRAMPROC setProgramProc;
	};

	class DrumsCtrl : public ModuleCtrl {
		GenericDrumCtrl* drumCtrls_[8];
	//protected: PROP_R(GenericDrumCtrl*, bassDrumCtrl);
	//protected: PROP_R(GenericDrumCtrl*, snareDrumCtrl);
	//protected: PROP_R(GenericDrumCtrl*, closedHihatCtrl);
	//protected: PROP_R(GenericDrumCtrl*, openHihatCtrl);
	//protected: PROP_R(GenericDrumCtrl*, lowTomCtrl);
	//protected: PROP_R(GenericDrumCtrl*, midTomCtrl);
	//protected: PROP_R(GenericDrumCtrl*, highTomCtrl);
	//protected: PROP_R(GenericDrumCtrl*, clapCtrl);

	public:
		DrumsCtrl(Module* module);
		~DrumsCtrl();

		GenericDrumCtrl** drumCtrl();

		void create(Window* parent, char* name);
		LRESULT onPaint();

		static SetSoundbankFunc soundbankSetter;
	};
}
#endif