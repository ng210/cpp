#include "winapp.h"
#include "base/debug.h"
#include "runtime.h"

NS_FW_BASE_USE
NS_FW_WIN_USE

// Entry point
extern INT WINAPI WinMain(HINSTANCE instance, HINSTANCE pprevInstance, char* cmdLine, int nShowCmd) {
	int error = 0;
#ifdef _DEBUG
	//MemoryMgr::isDebugOn = true;
	Debug::initialize(DEBUG_WINDOWS/* | DEBUG_UNICODE | DEBUG_MEMORY*/);
#endif
	//RunTime::initialize();

	//Map* args = NEW_(Map, MAP_USE_REF, MAP_USE_REF, Map::hashingStr, Collection::compareStr);
	//GetModuleFileNameA()
	//PathInfo* workingDir = NEW_(PathInfo, ???);
	//workingDir_ = workingDir->path();
	//for (int i = 0; i < argc; i++) {
	//	int count = 0;
	//	size_t pos = strcspn(argv[i], "=");
	//	char* key = substr(argv[i], 0, pos);
	//	char* value = (argv[i][pos] != '\0') ? substr(argv[i], pos + 1, NS_FW_BASE::strlen(argv[i])) : NS_FW_BASE::strdup("1");
	//	args->put(key, value);
	//}
	//DEL_(workingDir);

	// get command line arguments
	char** args_ = str_split(cmdLine, " ");
	Map* args = NEW_(Map, MAP_USE_REF, MAP_USE_REF, Map::hashingStr, Collection::compareStr);
	for (int i = 0; args_[i] != NULL; i++) {
		size_t pos = strcspn(args_[i], "=");
		char* key = substr(args_[i], 0, pos);
		char* value = (args_[i][pos] != '\0') ? substr(args_[i], pos + 1, NS_FW_BASE::strlen(args_[i])) : NS_FW_BASE::strdup("1");
		args->put(key, value);
	}
	FREE(args_);

	WinApp* winApp = createApplication(instance, args);
	if (winApp != NULL && winApp->hWnd() != NULL) {
		ShowWindow(winApp->hWnd(), SW_SHOWDEFAULT);
		SYSPR(UpdateWindow(winApp->hWnd()));
		winApp->main(args);
	}

	DEL_(winApp);

	MAP_FOREACH(args, FREE(key); FREE(value););
	DEL_(args);

	//RunTime::shutDown();
#ifdef _DEBUG
	MemoryMgr::checkMemDbgInfo(0, NULL);
#endif
	return error;
}

WinApp::WinApp() {
	this->workingDir_ = "";
}

WinApp::~WinApp() {
}

HWND WinApp::create(CREATESTRUCT* createStruct, WNDCLASSEX* wndClassEx) {
	return Window::create(createStruct, NULL, wndClassEx);
}

int WinApp::main(NS_FW_BASE::Map* args) {
	MSG msg;
	int isExiting = 0;
	while (!isExiting) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			//LOG("msg: %08x\n", msg.message);
			if (msg.message != WM_QUIT) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			} else {
				isExiting = true;
			}
		}
		update();
	}
	return (int)msg.wParam;
}

void WinApp::update() {
	Sleep(1);
}
