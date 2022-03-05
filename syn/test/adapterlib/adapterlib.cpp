#include "adapterlib.h"
#include "base/memory.h"
#include "collection/array.h"
#include "ssn1k/synthadapter.h"

NS_FW_BASE_USE
NS_PLAYER_USE
NS_SSN1K_USE

//void addAdapter(IAdapter* adapter) {
//	AdapterLib::addAdapter(adapter);
//}

IAdapter* getAdapter(int uid) {
	return AdapterLib::getAdapter(uid);
}

Map& getAdapters() {
	return AdapterLib::getAdapters();
}

BOOL WINAPI DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved
) {
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			// Initialize once for each new process.
			// Return FALSE to fail DLL load.
			// prepare adapters list
			AdapterLib::addAdapter(NEW_(SynthAdapter));
			break;

		case DLL_THREAD_ATTACH:
			// Do thread-specific initialization.
			break;

		case DLL_THREAD_DETACH:
			// Do thread-specific cleanup.
			break;

		case DLL_PROCESS_DETACH:
			// Perform any necessary cleanup.
			break;
	}
	return TRUE;
}
