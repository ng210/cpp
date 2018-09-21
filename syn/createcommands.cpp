#include "synapp.h"
#include "resource.h"

BOOL SynApp::processCreateCommands(UINT32 cmd, WPARAM wParam, LPARAM lParam) {
	BOOL res = false;
	int id = (wParam & 0xffff)- ID_CREATE;
	if (id >= 0 && id < 0x0100) {
		int adapterId = (id >> 8) & 0xff;
		int targetId = wParam & 0xff;
		ADAPTER_INFO* info = (ADAPTER_INFO*)SynApp::adapterInfos_.getAt(adapterId);
		ADAPTER_TARGET_TYPE* targetType = (ADAPTER_TARGET_TYPE*)info->targetTypes->values()->getAt(targetId);

		IAdapter* adapter = (IAdapter*)SynApp::adapters_.getAt(adapterId);
		Target* target = adapter->createTarget(targetType->id, NULL);
		DEBUG("Target '%s' for '%s' was created\n", targetType->name, target->adapter->getInfo()->name);

		res = true;
	}
	return res;
}
