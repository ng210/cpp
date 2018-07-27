#include "synapp.h"
#include "resource.h"
//#include "bank.h"
//#include "seqseries.h"
#include "utils/utils.h"
#include "soundlib/soundplayer.h"

const int SynApp::sampleRate_ = 48000;

SynApp* SynApp::instance_ = NULL;

WinApp* createApplication(HINSTANCE hInstance, Map* args) {
	// Load menu
	HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDC_MAINMENU));

	CREATESTRUCT createStruct = {
		NULL,							// LPVOID      lpCreateParams;
		hInstance,						// HINSTANCE   hInstance;
		hMenu,							// HMENU       hMenu;
		NULL,							// HWND        hwndParent;
		480,							// int         cy;
		640,							// int         cx;
		CW_USEDEFAULT,					// int         y;
		CW_USEDEFAULT,					// int         x;
		WS_OVERLAPPEDWINDOW,			// LONG        style;
		"Syn Editor",					// LPCSTR      lpszName;
		0,								// LPCSTR      lpszClass;
		0								// DWORD       dwExStyle;
	};
	WNDCLASSEX wndClassEx = {
		sizeof(WNDCLASSEX),				// UINT        cbSize;
										/* Win 3.x */
										CS_HREDRAW | CS_VREDRAW,		// UINT        style;
										NULL,							// WNDPROC     lpfnWndProc;
										0,								// int         cbClsExtra;
										0,								// int         cbWndExtra;
										hInstance,						// HINSTANCE   hInstance;
										LoadIcon(NULL, IDI_APPLICATION),// HICON       hIcon;
										LoadCursor(NULL, IDC_ARROW),	// HCURSOR     hCursor;
										(HBRUSH)COLOR_BACKGROUND,		// HBRUSH      hbrBackground;
										NULL,							// LPCSTR      lpszMenuName;
										"SynWClass",					// LPCSTR      lpszClassName;
																		/* Win 4.0 */
																		NULL							// HICON       hIconSm;
	};
	return SynApp::createInstance(&createStruct, &wndClassEx);
}

SynApp::SynApp(CREATESTRUCT* createStruct, WNDCLASSEX* wndClassEx) {
	// put initialization code here
	synthAdapter_ = NULL;
	player_ = NULL;
	isPlaying_ = false;
	// allocate resources
	log_.append("ctor\n", 5);
	create(createStruct, wndClassEx);
}

SynApp::~SynApp() {
	if (isPlaying_) {
		SoundPlayer::stop();
	}
	log_.append("dtor\n", 5);
	// free resources
	DEL_(asu_);
	DEL_(player_);
}

LRESULT CALLBACK SynApp::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT ret = 0;
	switch (uMsg) {
	case WM_GETMINMAXINFO:
		MINMAXINFO* mmi;
		mmi = (MINMAXINFO*)lParam;
		mmi->ptMaxSize.x = 1920; mmi->ptMaxSize.y = 1280;
		//mmi->ptMaxPosition.x = 20; mmi->ptMaxPosition.y = 20;
		mmi->ptMaxTrackSize.x = 1920; mmi->ptMaxTrackSize.y = 1280;
		mmi->ptMinTrackSize.x = 320; mmi->ptMinTrackSize.y = 200;
		break;
	case WM_SIZE:
		RECT rect;
		SYSPR(GetClientRect(hWnd_, &rect));
		rect.left += 2; rect.right -= 4;
		rect.top += 2; rect.bottom -= 4;
		//SYSFN(ret, SetWindowPos(multiChartView_->hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
		break;
	case WM_LBUTTONUP:
		if (isPlaying_) {
			SoundPlayer::stop();
		}
		else {
			SoundPlayer::start(sampleRate_, 1, soundCallback);
		}
		break;

	default:
		ret = Window::wndProc(hWnd, uMsg, wParam, lParam);
	}
	return ret;
}

SynApp* SynApp::createInstance(CREATESTRUCT* createStruct, WNDCLASSEX* wndClassEx) {
	SynApp::instance_ = NEW_(SynApp, createStruct, wndClassEx);
	return SynApp::instance_;
}

LRESULT SynApp::onCreate() {
	//SYSPR(SetWindowText(hWnd_, "MultiChartView test"));
	asu_ = NEW_(Asu);
	createTestSong();
	return 0;
}

LRESULT SynApp::onCommand(WPARAM wParam, LPARAM lParam) {
	LRESULT res = 0;
	WORD cmd = LOWORD(wParam);
	switch (cmd) {
		// File
		case ID_FILE_OPEN:
			openFile();
			break;
		case ID_FILE_SAVE:
			saveFile();
			break;
		case ID_FILE_SAVEAS:
			saveFileAs();
			break;
		case ID_FILE_IMPORT:
			importFile();
			break;
		case ID_FILE_EXPORT:
			exportFile();
			break;
		case ID_FILE_EXIT:
			PostQuitMessage(0);
			break;
		// Edit
		case ID_EDIT_ADDADAPTER:
			addAdapter();
			break;
		case ID_EDIT_ADDSEQUENCE:
			addSequence();
			break;
		case ID_EDIT_ADDUSERDATA:
			addUserDataBlock();
			break;
			

		// Play
		case ID_PLAY_START:
			if (isPlaying_) {
				res = SoundPlayer::stop();
				// reset ssn
			}
			else {
				if (asu_ != NULL) {
					res = SoundPlayer::start(sampleRate_, 1, soundCallback);
				}
			}
			isPlaying_ = !isPlaying_;
			break;
	}
	return res;
}

#include "bank.h"

static int createInitDataForSynthAdapter(out void*& data) {
	// create init data for 16 synths
	UINT32 length = 1 + 16 + 1;
	int instrumentCount = arraysize(instruments_);
	for (int i = 0; i < instrumentCount; i++) {
		length += instrumentSizes_[i];
	}
	UINT8* ptr = MALLOC(UINT8, length);
	data = ptr;
	*ptr++ = instrumentCount;
	for (int i = 0; i < instrumentCount; i++) {
		int size = instrumentSizes_[i];
		memcpy(ptr, instruments_[i], size); ptr += size;
	}
	// use 4 synths
	*ptr++ = 4;
	UINT8 voiceCount[16] = { 3, 2, 1, 2,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1 };
	length = sizeof(voiceCount);
	memcpy(ptr, voiceCount, length); ptr += length;
	return length;
}

static char* testSongPath = "min.xm";

void SynApp::createTestSong() {
	Player* ply = asu_->player();
	// create synth adapter
	SynthAdapter* adapter = NEW_(SynthAdapter);
	// create init data
	void* ptr = NULL;
	int length = createInitDataForSynthAdapter(ptr);
	// add adapter and initialization data
	asu_->addAdapter(adapter, length, ptr);
	// read xm and import sequences
	int start = 0;
	if (asu_->importXm(testSongPath, start) == 0) {
		// import error
		MessageBox(hWnd_, "File read import error!", "Error", MB_OK);
	}

	//// add user data blocks:
	////  - create 16 synths
	//UINT32 length = 1 + 16 * 2;
	//UINT8* ptr = MALLOC(UINT8, length);
	//UINT8 voiceCount[16] = { 3, 2, 1, 2,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1 };
	//for (int i = 0; i < 16; i++) {
	//	*ptr++ = voiceCount[i];	// voice count
	//	*ptr++ = 1;				// user block id for sound bank
	//}
	//ply->addUserDataBlock(length, ptr);
	////  - create sound bank
	//SYNTH_BANK* bank = MALLOC(SYNTH_BANK, 1);
	//bank->instrumentCount = 4;
	//bank->instrumentData = instruments;
	//ply->addUserDataBlock(sizeof(bank), bank);

	//// add synthadapter with user data block #0
	//SynthAdapter* adapter = NEW_(SynthAdapter);
	//adapter->initialize(0, ply);
	//ply->addAdapter(adapter);
}