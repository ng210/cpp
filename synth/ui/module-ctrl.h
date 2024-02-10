#ifndef __MODULE_CTRL_H
#define __MODULE_CTRL_H

#include "win/buttonctrl.h"
#include "win/comboboxctrl.h"
#include "win/staticctrl.h"
#include "synth/ui/pot-ctrl.h"
#include "synth/ui/device-ctrl.h"
#include "synth/src/device/module-device.h"
#include "base/stream.h"
#include "player/src/sequence.h"

NS_FW_BASE_USE
NS_FW_WIN_USE
using namespace SYNTH;
using namespace PLAYER;

namespace SYNTH_APP {

	typedef struct LayoutItem {
		byte ctrlId;
		char label[16];
		PotCtrlType type;
	} LayoutItem;

	typedef enum LayoutControl {
		LayoutEnd = 255,
		LayoutHorizontalGap = 254,
		LayoutVerticalGap = 253,
		LayoutNextColumn = 252,
		LayoutBlank = 251,
		LayoutNewColumn = 250,
	} LayoutControl;

	class ModuleCtrl : public DeviceCtrl {
		static WndClass wndClass_;
		static CBSELECTITEMPROC onSelectProgram;
		static HANDLE hBackground_;
	protected:
		int potCtrlCount_;
		ComboboxCtrl programCtrl_;
		ButtonCtrl addButton_;
		ButtonCtrl removeButton_;
		StaticCtrl background_;
	protected: PROP(bool, hasLabel);
	protected: PROP_R(PotCtrl**, potCtrls);
	protected: PROP_R(Module*, module);
	protected: PROP_R(int, borderWidth);
	protected: PROP_R(int, gapWidth);
	public:
		ModuleCtrl(Module* module);
		virtual ~ModuleCtrl();

		virtual void create(Window* parent, char* name);
		virtual void initFromStream(Stream* data, int size = 100, unsigned long *colors = NULL);
		virtual HANDLE getBackgroundImage();
		
		LRESULT onSize(RECT& rect, WPARAM state);

		void updateSoundbank();

		PotCtrl* getControl(int id);

		static MOUSEEVENTPROC onAddProgram, onRemoveProgram;

		static int soundbankSetter(void*, Soundbank*, void* = NULL);
		static int programSetter(void*, int, void* = NULL);

		//static SETCONTROLPROC setControlProc;
		//static SETSOUNDBANKPROC setSoundbankProc;
		//static SETPROGRAMPROC setProgramProc;
		//static CBADDITEMPROC addItemProc;
		//static CBSELECTITEMPROC selectItemProc;
	};
}
#endif
