#ifndef __SYNTH_APP_H
#define __SYNTH_APP_H

#include "win/winapp.h"
#include "player/src/player-lib.h"
#include "player/src/player-adapter.h"
#include "synth/ui/pot-ctrl.h"
#include "synth/ui/synth-ctrl.h"
#include "synth/ui/bass-ctrl.h"
#include "synth/ui/drums-ctrl.h"
#include "synth/ui/distort-ctrl.h"
#include "synth/ui/stereo-delay-ctrl.h"
#include "synth/ui/mixer-ctrl.h"
#include "synth/src/device/synth-adapter.h"
#include "synth/src/device/mixer-device.h"


NS_FW_BASE_USE
NS_FW_WIN_USE
using namespace SYNTH_UI;

class SynthApp : public WinApp {
	PlayerAdapter playerAdapter_;
	SynthAdapter synthAdapter_;
	PlayerDevice* masterDevice_;
	Player* player_;
	Mixer8x4* mixer_;

	PArray ctrls_;
	Map soundbanks_;
public:
	SynthApp();
	~SynthApp();

	void create(WndClass wndClass, LONG style = 0, DWORD exStyle = 0);

	LRESULT onCreate();

	int main(NS_FW_BASE::Map* args);

	Stream* createBinaryData();
	void loadBinary();
	void loadSoundbanks();
	void saveSoundbanks();
};

#endif
