#ifndef __SYNTH_APP_H
#define __SYNTH_APP_H

#include "win/winapp.h"
#include "player/src/player-lib.h"
#include "player/src/player-adapter.h"
#include "synth/ui/pot-ctrl.h"
#include "synth/ui/synth-ctrl.h"
#include "synth/ui/synth2-ctrl.h"
#include "synth/ui/bass-ctrl.h"
#include "synth/ui/drums-ctrl.h"
#include "synth/ui/distort-ctrl.h"
#include "synth/ui/stereo-delay-ctrl.h"
#include "synth/ui/mixer-ctrl.h"
#include "synth/src/device/synth-adapter.h"
#include "synth/src/device/mixer-device.h"

#include "synth/ui/ui-manager.h"
#include "synth/ui/file-manager.h"
#include "synth/ui/creator.h"
#include "synth/ui/song.h"

NS_FW_BASE_USE
NS_FW_WIN_USE
using namespace SYNTH_APP;

namespace SYNTH_APP {

	class SynthApp : public WinApp {
		// main components
		UiManager* uiManager_;
		FileManager* fileManager_;
		Creator* creator_;

		PROP_R(PlayerAdapter, playerAdapter);
		PROP_R(SynthAdapter, synthAdapter);
		PROP_R(PArray, songs);
		PROP_R(Song*, activeSong);
		PROP_R(Player*, activePlayer);

	public:
		SynthApp();
		~SynthApp();

		void create(WndClass wndClass);
		//LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		LRESULT onCreate();
		LRESULT onSize(RECT& rect, WPARAM state);

		#pragma region Functions
		#pragma region File menu
		// - New...
		void createNewSong();
		// - Open...
		void loadSong();
		// - Save...
		void saveSong();
		// - Quit
		void closeSong();
		#pragma endregion

		#pragma region Edit menu
		// - Add device
		int addDevice(int type);
		// - Add sequence
		int addSequence(int deviceId);
		// - Copy
		int copyDevice(int deviceId);
		int copySequence(int sequenceId);
		// - Delete
		int removeDevice(int deviceId);
		int removeSequence(int sequenceId);
		#pragma endregion

		#pragma region View menu
		// - View Device
		void openDeviceEditor(int deviceId);
		// - View Sequence
		void openSequenceEditor(int sequenceId);
		#pragma endregion

		#pragma region Playback menu
		// - Start
		void startPlayback();
		// - Stop
		void stopPlayback();
		#pragma endregion
		#pragma endregion

		int main(NS_FW_BASE::Map* args);
	};
}

#endif
