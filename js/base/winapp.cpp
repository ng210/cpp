#include "winapp.h"
#include "base/debug.h"
#include "runtime.h"

NS_FW_BASE_USE

extern int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow) {
	int ret = 0;
#ifdef _DEBUG
	tracer_ = new TracerWin();
#endif
	RunTime::initialize();

	String* str = NEW_(String, cmdLine);
	Array* arr = str->split(" ");
	DEL_(str);
	String* argSeparator = NEW_(String, "=");
	Map* args = NEW_(Map, arr, argSeparator);
	DEL_(argSeparator);
	arr->cleanUp();
	DEL_(arr);

	ret = winMain(instance, args);

	args->keys()->cleanUp();
	args->values()->cleanUp();
	args->cleanUp();
	DEL_(args);

	RunTime::shutDown();
#ifdef _DEBUG
	MemoryMgr::checkMemDbgInfo(0, NULL);
	delete tracer_;
#endif
	return ret;
}

extern int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow) {
#ifdef _DEBUG
	tracer_ = NEW_(TracerWin);
#endif
	RunTime::initialize();
	// workaround convert to mb string
	char buffer[512];
	wcstombs(buffer, cmdLine, 512);
	String str((const char*)buffer);
	Array* arr = str.split(" ");
	String* argSeparator = NEW_(String, "=");
	Map* args = NEW_(Map, arr, argSeparator);
	DEL_(argSeparator);
	arr->cleanUp();
	DEL_(arr);
	int ret = winMain(instance, args);
#ifdef _DEBUG
	DEL_(tracer_);
#endif
	return ret;
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