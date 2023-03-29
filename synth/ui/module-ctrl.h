#ifndef __MODULE_CTRL_H
#define __MODULE_CTRL_H

#include "win/ctrl.h"
#include "win/buttonctrl.h"
#include "win/comboboxctrl.h"
#include "synth/ui/pot-ctrl.h"
#include "synth/src/module/module.h"
#include "base/stream.h"

NS_FW_BASE_USE
NS_FW_WIN_USE
using namespace SYNTH;

namespace SYNTH_UI {

	typedef struct LayoutItem {
		byte ctrlId;
		char label[16];
		PotCtrlType type;
	} LayoutItem;

	typedef enum LayoutControl {
		LayoutEnd = 255,
		LayoutHorizontalGap = 254,
		LayoutVerticalGap = 253
	} LayoutControl;

	class ModuleCtrl : public Ctrl {
		static WndClass wndClass_;
		static CBSELECTITEMPROC onSelectProgram;
	protected:
		int potCtrlCount_;
		ComboboxCtrl programCtrl_;
		ButtonCtrl addButton_;
		ButtonCtrl removeButton_;
	//protected: PROP_R(ComboboxCtrl*, programSelect);
	protected: PROP_R(PotCtrl**, potCtrls);
	protected: PROP_R(Module*, module);
	protected: PROP_R(int, borderWidth);
	protected: PROP_R(int, gapWidth);
	public:
		ModuleCtrl(Module* module);
		virtual ~ModuleCtrl();

		virtual void create(Window* parent, char* name);
		virtual void initFromStream(Stream* data, int size = 128);

		//LRESULT onPaint();

		void updateSoundbank();
		//void updateProgram();

		PotCtrl* getControl(int id);

		static MOUSEEVENTPROC onAddProgram, onRemoveProgram;

		static SetSoundbankFunc soundbankSetter;
		static SetProgramFunc programSetter;


		//static SETCONTROLPROC setControlProc;
		//static SETSOUNDBANKPROC setSoundbankProc;
		//static SETPROGRAMPROC setProgramProc;
		//static CBADDITEMPROC addItemProc;
		//static CBSELECTITEMPROC selectItemProc;
	};
}
#endif
