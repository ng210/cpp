#include "synapp.h"
#include "resource.h"
#include "utils/utils.h"
#include "utils/directory.h"

PArray SynApp::adapters_;
PArray SynApp::adapterInfos_;
Array SynApp::createMenuIds_(sizeof(UINT32));

WinApp* instance_ = NULL;

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
	instance_ = NEW_(SynApp, &createStruct, &wndClassEx);
	return instance_;
}

SynApp::SynApp(CREATESTRUCT* createStruct, WNDCLASSEX* wndClassEx) {
	// put initialization code here
	player_ = NULL;
	// allocate resources
	log_.append("ctor\n", 5);
	create(createStruct, wndClassEx);
}

SynApp::~SynApp() {
	log_.append("dtor\n", 5);
	// free resources
	DEL_(asu_);
	DEL_(player_);
}

/// <summary>
/// Read DLL files from plugin folder
/// Activate each plugin by calling the entry function
/// Register adapters defined by the plugins
/// </summary>
void SynApp::registerPlugins() {
	const char* dirName = "test\\plugins\\";
	size_t dirNameLength = NS_FW_BASE::strlen(dirName);
	// Get adapter plugins
	Array* files = Directory::getFiles(dirName, ".dll");
	for (UINT32 i = 0; i < files->length(); i++) {
		char* fileName = (char*)files->getAt(i);
		char path[MAX_PATH];
		strncpy(path, dirNameLength, dirName);
		strncpy(&path[dirNameLength], MAX_PATH - dirNameLength, fileName);
		HMODULE SYSFN(lib, LoadLibrary(path));
		// implicit call to DllMain
		GETADAPTERINFOS* SYSFN(getAdapterInfos, (GETADAPTERINFOS*)GetProcAddress(lib, "getAdapterInfos"));
		if (getAdapterInfos != NULL) {
			// get all adapters
			Array* adapterInfos = getAdapterInfos();
			for (UINT32 j = 0; j < adapterInfos->length(); j++) {
				ADAPTER_INFO* info = (ADAPTER_INFO*)adapterInfos->getAt(j);
				info->initialize();
				Asu::registerAdapter(info);
				SynApp::adapterInfos_.add(info);
				SynApp::adapters_.add(info->create(NULL));
				// get all target types
				for (UINT32 k = 0; k < info->targetTypes->values()->length(); k++) {
					ADAPTER_TARGET_TYPE* targetType = (ADAPTER_TARGET_TYPE*)info->targetTypes->values()->getAt(k);
					UINT32 key = (j << 8) | k;
					createMenuIds_.add(&key);
				}
			}
		}
	}
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
		//case WM_MENUCOMMAND:
		//	// menu items added dynamically
		//	onMenuCommand((int)wParam, (HMENU)lParam);
		//	break;

	default:
		ret = Window::wndProc(hWnd, uMsg, wParam, lParam);
	}
	return ret;
}

void SynApp::createMenu() {
	// build "Create" menu
	HMENU SYSFN(hMenu, GetMenu(hWnd_));
	HMENU SYSFN(hCreateMenu, CreateMenu());
	MENUITEMINFO mii;
	memset(&mii, 0, sizeof(MENUITEMINFO));
	mii.cbSize = sizeof(MENUITEMINFO);
	char menuItemString[64];
	mii.fMask = MIIM_SUBMENU;
	mii.hSubMenu = hCreateMenu;
	BOOL SYSFN(success, SetMenuItemInfo(hMenu, ID_CREATE, false, &mii));

	if (success) {
		int adapterId = -1;
		ADAPTER_INFO* info = NULL;
		int adapterCount = 0;
		int targetCount = 0;
		HMENU hSubmenu;
		for (UINT32 i = 0; i < SynApp::createMenuIds_.length(); i++) {
			UINT32 id = *(UINT32*)SynApp::createMenuIds_.getAt(i);
			UINT32 newAdapterId = id >> 8;
			if (adapterId != newAdapterId) {
				adapterId = newAdapterId;
				info = (ADAPTER_INFO*)SynApp::adapterInfos_.getAt(adapterId);
				// add submenu
				SYSFN(hSubmenu, CreateMenu());
				// add submenu to create menu
				mii.fMask = MIIM_TYPE | MIIM_ID | MIIM_SUBMENU;
				mii.fType = MFT_STRING;
				mii.wID = info->id;
				mii.dwTypeData = info->name;
				mii.hSubMenu = hSubmenu;
				SYSPR(InsertMenuItem(hCreateMenu, adapterCount++, true, &mii));
				targetCount = 0;
			}
			int targetId = id & 0xff;
			ADAPTER_TARGET_TYPE* targetType = (ADAPTER_TARGET_TYPE*)info->targetTypes->values()->getAt(targetId);
			mii.fMask = MIIM_TYPE | MIIM_ID;
			mii.fType = MFT_STRING;
			mii.wID = ID_CREATE + i;
			mii.dwTypeData = targetType->name;
			SYSPR(InsertMenuItem(hSubmenu, targetCount++, true, &mii));
		}

		//// get all adapters
		//UINT32 SYSFN(ix, GetMenuItemCount(hCreateMenu));
		//Array* infos = Asu::getAdapterInfoList();
		//for (UINT32 i = 0; i < infos->length(); i++) {
		//	ADAPTER_INFO* info = (ADAPTER_INFO*)infos->getAt(i);
		//	// add menu entries
		//	HMENU SYSFN(hSubmenu, CreateMenu());
		//	// add submenu to create menu
		//	mii.fMask = MIIM_TYPE | MIIM_ID | MIIM_SUBMENU;
		//	mii.fType = MFT_STRING;
		//	mii.wID = info->id;
		//	mii.dwTypeData = info->name;
		//	mii.hSubMenu = hSubmenu;
		//	SYSPR(InsertMenuItem(hCreateMenu, ix++, true, &mii));
		//	for (UINT32 j = 0; j < info->targetTypes->values()->length(); j++) {
		//		UINT32 key = j + (i<<8);
		//		ADAPTER_TARGET_TYPE* targetType = (ADAPTER_TARGET_TYPE*)info->targetTypes->values()->getAt(j);
		//		mii.fMask = MIIM_TYPE | MIIM_ID;
		//		mii.fType = MFT_STRING;
		//		mii.wID = ID_CREATE + j;
		//		mii.dwTypeData = targetType->name;
		//		SYSPR(InsertMenuItem(hSubmenu, j, true, &mii));
		//	}
		//}
		//DEL_(infos);
	}
}

LRESULT SynApp::onCreate() {
	registerPlugins();
	createMenu();
	//SYSPR(SetWindowText(hWnd_, "MultiChartView test"));
	asu_ = NEW_(Asu);
	return 0;
}

LRESULT SynApp::onCommand(WPARAM wParam, LPARAM lParam) {
	LRESULT res = 1;
	WORD cmd = LOWORD(wParam);
	while (true) {
		if (processFileCommands(cmd, wParam, lParam)) break;
		if (processEditCommands(cmd, wParam, lParam)) break;
		if (processCreateCommands(cmd, wParam, lParam)) break;
		// else
		res = 0;
		break;
	}
	return res;
}