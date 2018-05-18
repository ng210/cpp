#include "winapp.h"
#include "base/debug.h"
#include "runtime.h"

NS_FW_BASE_USE

static char* workingDir_;

extern int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow) {
	int error = 0;
#ifdef _DEBUG
	//MemoryMgr::isDebugOn = true;
	Debug::initialize(/*DEBUG_UNICODE | DEBUG_MEMORY*/);
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

	char** args_ = str_split_(cmdLine, " ");
	Map* args = NEW_(Map, MAP_USE_REF, MAP_USE_REF, Map::hashingStr, Map::compareStr);
	for (int i = 0; args_[i] != NULL; i++) {
		size_t pos = strcspn(args_[i], "=");
		char* key = substr(args_[i], 0, pos);
		char* value = (args_[i][pos] != '\0') ? substr(args_[i], pos + 1, NS_FW_BASE::strlen(args_[i])) : NS_FW_BASE::strdup("1");
		args->put(key, value);
	}

	error = winMain(instance, args);
	MAP_FOREACH(args, FREE(key); FREE(value));
	DEL_(args);

	//RunTime::shutDown();
#ifdef _DEBUG
	MemoryMgr::checkMemDbgInfo(0, NULL);
#endif
	return error;
}

extern int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow) {
	int error = 0;
#ifdef _DEBUG
	//MemoryMgr::isDebugOn = true;
	Debug::initialize(/*DEBUG_UNICODE | DEBUG_MEMORY*/);
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

	char** args_ = str_split_(cmdLine, " ");
	Map* args = NEW_(Map, MAP_USE_REF, MAP_USE_REF, Map::hashingStr, Map::compareStr);
	for (int i = 0; args_[i] != NULL; i++) {
		size_t pos = strcspn(args_[i], "=");
		char* key = substr(args_[i], 0, pos);
		char* value = (args_[i][pos] != '\0') ? substr(args_[i], pos + 1, NS_FW_BASE::strlen(args_[i])) : NS_FW_BASE::strdup("1");
		args->put(key, value);
	}

	error = winMain(instance, args);
	MAP_FOREACH(args, FREE(key); FREE(value));
	DEL_(args);

	//RunTime::shutDown();
#ifdef _DEBUG
	MemoryMgr::checkMemDbgInfo(0, NULL);
#endif
	return error;
}

NS_FW_BASE_BEGIN

ATOM WinApp::windowClass_ = 0;

WinApp::~WinApp() {

}

HACCEL WinApp::getAcceleratorTable() {
	return NULL;
}
int WinApp::main(LPCWSTR cmdLine) {
	HINSTANCE instance = SYS(GetModuleHandle(NULL));
	HACCEL hAccelTable = SYS(getAcceleratorTable());
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!hAccelTable || !TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}

NS_FW_BASE_END