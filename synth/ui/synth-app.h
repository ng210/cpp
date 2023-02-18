#ifndef __SYNTH_APP_H
#define __SYNTH_APP_H

#include "win/winapp.h"
#include "synth/ui/pot-ctrl.h"
#include "synth/ui/bass-ctrl.h"

NS_FW_BASE_USE
NS_FW_WIN_USE
using namespace SYNTH_UI;

class SynthApp : public WinApp {
	BassCtrl* bassCtrl_;
public:
	SynthApp();
	~SynthApp();

	void create(WndClass wndClass, LONG style = 0, DWORD exStyle = 0);

	LRESULT onCreate();

	void createBassModuleCtrl();

	int main(NS_FW_BASE::Map* args);
};

#endif
