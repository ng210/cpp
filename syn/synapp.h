#ifndef __SYNAPP_H
#define __SYNAPP_H

#include "winapp.h"
#include "asu.h"
#include "utils/buffer.h"

//#include "multichartview/multichartview.h"
//#include "seqseries.h"
//
//#include "soundlib/SoundPlayer.h"
//#include "soundlib/WavFmt.h"
//
//#include "ssn1k/ssn1klib.h"
#include "ssn1k/synthadapter.h"
//
//#include "syn/synplayer.h"
//#include "player/playeradapter.h"
//#include "syn/synsequence.h"
//#include "player/channel.h"
//#include "xmloader.h"

NS_FW_BASE_USE
NS_SSN1K_USE
//NS_PLAYER_USE

void soundCallback(short* buffer, int sampleCount);

class SynApp : public WinApp {
	static const int sampleRate_;
	static SynApp* instance_;
	Player* player_;
	//PlayerAdapter* playerAdapter_;
	//PArray* sequences_;
	//PArray* series_;

	bool isPlaying_;

	Asu* asu_;

	SynthAdapter* synthAdapter_;
	//Synth* synths[16];
	//Mixer* mixer;

	//MultiChartView* multiChartView_;
	//DataSeries* dataSeries_;

	Buffer log_;

	void setupSound();

	LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	SynApp(CREATESTRUCT* createStruct, WNDCLASSEX* wndClassEx);
public:
	static SynApp* createInstance(CREATESTRUCT* createStruct, WNDCLASSEX* wndClassEx);
	static SynApp* instance() {
		return SynApp::instance_;
	}
	~SynApp();

	LRESULT onCreate();
	LRESULT onCommand(WPARAM wParam, LPARAM lParam);

	void feedSample(short* buffer, int sampleCount);

	// file menu
	LRESULT openFile();
	LRESULT saveFile();
	LRESULT saveFileAs();
	LRESULT importFile();
	LRESULT exportFile();

	// edit menu
	LRESULT addAdapter();
	LRESULT addSequence();
	LRESULT addUserDataBlock();


	void createTestSong();


	//PArray* importSequencesFromXM(const char* path);
	//void freeSequences(PArray* sequences);
	//void createSeries(PArray* sequences);
	//void prepareViews();
	//void preparePlayer();
	//	
	//PArray* createSequences();

};

#endif