#ifndef __ADAPTER_LIB_H
#define __ADAPTER_LIB_H

#include <windows.h>
#include "player/iadapter.h"

NS_FW_BASE_USE
NS_PLAYER_USE

#ifdef __cplusplus
extern "C" {
#endif

	__declspec(dllexport) ADAPTER_INFO* addAdapter(IAdapter* adapter);
	__declspec(dllexport) Array* getAdapterInfos();

	BOOL WINAPI DllMain(
		_In_ HINSTANCE hinstDLL,
		_In_ DWORD     fdwReason,
		_In_ LPVOID    lpvReserved
	);

#ifdef __cplusplus
}
#endif

#endif