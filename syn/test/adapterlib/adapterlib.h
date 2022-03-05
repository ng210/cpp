#ifndef __ADAPTER_LIB_H
#define __ADAPTER_LIB_H

#include <windows.h>
#include "player/adapterlib.h"

NS_FW_BASE_USE
NS_PLAYER_USE

#ifdef __cplusplus
extern "C" {
#endif

	//__declspec(dllexport) void addAdapter(IAdapter* adapter);
	__declspec(dllexport) IAdapter* getAdapter(int uid);
	__declspec(dllexport) Map& getAdapters();

	BOOL WINAPI DllMain(
		_In_ HINSTANCE hinstDLL,
		_In_ DWORD     fdwReason,
		_In_ LPVOID    lpvReserved
	);

#ifdef __cplusplus
}
#endif

#endif