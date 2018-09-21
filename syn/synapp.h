#ifndef __SYNAPP_H
#define __SYNAPP_H

#include "winapp.h"
#include "asu.h"
#include "utils/buffer.h"

NS_FW_BASE_USE
NS_PLAYER_USE

WinApp* createApplication(HINSTANCE hInstance, Map* args);

extern WinApp* instance_;

/// <summary>
/// Application for creating and editing asu content.
/// Main functions
///  - load adapters dynamically
///  - create sequences, commands defined by adapters
///  - create objects via adapters
///  - create user data blocks via adapters
/// </summary>
class SynApp : public WinApp {
	static PArray adapters_;
	static PArray adapterInfos_;
	static Array createMenuIds_;
	// main document
	Asu* asu_;
	// main playback
	Player* player_;

	// register plugins of adapters
	void registerPlugins();

	//MultiChartView* multiChartView_;
	//DataSeries* dataSeries_;

	Buffer log_;

	LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	// Menu handling
	void createMenu();
	// file menu
	BOOL processFileCommands(UINT32 cmd, WPARAM wParam, LPARAM lParam);
	LRESULT openFile();
	LRESULT saveFile();
	LRESULT saveFileAs();
	LRESULT importFile();
	// edit menu
	BOOL processEditCommands(UINT32 cmd, WPARAM wParam, LPARAM lParam);
	// create menu
	BOOL processCreateCommands(UINT32 cmd, WPARAM wParam, LPARAM lParam);

public:
	SynApp(CREATESTRUCT* createStruct, WNDCLASSEX* wndClassEx);
	~SynApp();

	LRESULT onCreate();
	LRESULT onCommand(WPARAM wParam, LPARAM lParam);

	//LRESULT exportFile();

	// edit menu
	//LRESULT copy();
	//LRESULT cut();
	//LRESULT paste();
	//LRESULT undo();
	//LRESULT redo();

	// create
	// adapters are submenus
	// sub-submenus
	LRESULT addTarget(int id);
	LRESULT addSequence(int id);
	LRESULT addUserDataBlock(int id);
};

#endif