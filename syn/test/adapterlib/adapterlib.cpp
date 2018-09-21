#include "adapterlib.h"
#include "collection/array.h"
#include "base/memory.h"
#include "ssn1k/synthadapter.h"

NS_FW_BASE_USE
NS_PLAYER_USE
NS_SSN1K_USE

static PArray* adapters_ = NULL;
static Array* adapterInfos_ = NULL;

ADAPTER_INFO* addAdapter(IAdapter* adapter) {
	if (adapters_ == NULL) {
		adapters_ = NEW_(PArray);
		adapterInfos_ = NEW_(Array, sizeof(ADAPTER_INFO));
	}
	adapters_->add(adapter);
	return (ADAPTER_INFO*)adapterInfos_->add((void*)adapter->getInfo());
}

Array* getAdapterInfos() {
	return adapterInfos_;
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
			addAdapter(NEW_(SynthAdapter));
			break;

		case DLL_THREAD_ATTACH:
			// Do thread-specific initialization.
			break;

		case DLL_THREAD_DETACH:
			// Do thread-specific cleanup.
			break;

		case DLL_PROCESS_DETACH:
			// Perform any necessary cleanup.
			ARRAY_FOREACH(adapters_, DEL_((IAdapter*)value); );
			DEL_(adapters_);
			break;
	}
	return TRUE;
}
