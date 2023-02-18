#ifndef __MODULE_CTRL_H
#define __MODULE_CTRL_H

#include "win/ctrl.h"
#include "synth/ui/pot-ctrl.h"
#include "synth/src/module/module.h"
#include "base/stream.h"

NS_FW_BASE_USE
NS_FW_WIN_USE
using namespace SYNTH;

namespace SYNTH_UI {

	typedef struct LayoutItem {
		byte ctrlId;
		word x, y;
		char label[16];
		PotCtrlType type;
	} LayoutItem;

	class ModuleCtrl : public Ctrl {
		static WndClass wndClass_;
		int potCtrlCount_;
	protected: PROP_R(PotCtrl**, potCtrls);
	protected: PROP_R(POINT*, potPosition);
	protected: PROP_R(Module*, module);
	public:
		ModuleCtrl(Module* module);
		~ModuleCtrl();

		void create(Window* parent, char* name);

		virtual void initFromStream(Stream* data);

		LRESULT onPaint();
	};
}
#endif
