#ifndef __MULTICHARTVIEW_TB_H
#define __MULTICHARTVIEW_TB_H

#include "win/ctrl.h"
#include "win/comboboxctrl.h"

#define MULTICHARTVIEW_TOOLBARHEIGHT 24

NS_FW_WIN_BEGIN

#define MULTICHARTVIEW_TOOLBAR_WCLASS "MULTICHARTVIEWTOOLBAR"
#define ID_MULTICHARTVIEW_TOOLBAR_CHANNELSELECT 0x0201

class MultiChartViewToolbar : public Ctrl {
protected: PROP_REF(ComboboxCtrl*, channelSelect);
protected:
	static WINDOWCLASS toolbarWndClass_;
	LRESULT onCreate();
	LRESULT onCommand(WPARAM wParam, LPARAM lParam);
	//int onDestroy();
public:
	MultiChartViewToolbar(Window* parent, int ctrlId);
	~MultiChartViewToolbar();
};

NS_FW_WIN_END

#endif